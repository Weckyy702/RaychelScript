/**
* \file read.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for reading related functions
* \date 2021-12-17
*
* MIT License
* Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include "rasm/read.h"

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#define TRY_READ(out_type, out_name, error_return)                                                                               \
    out_type out_name{};                                                                                                         \
    if (const auto maybe_value = details::read<out_type>(stream); !maybe_value.has_value()) {                                    \
        return error_return;                                                                                                     \
    } else { /*NOLINT*/                                                                                                          \
        (out_name) = maybe_value.value();                                                                                        \
    }

namespace RaychelScript::Assembly {

    namespace details {

        template <typename T>
        std::optional<T> read(std::istream&) noexcept;

        template <typename T>
        requires std::is_trivial_v<T> std::optional<T> read(std::istream& stream)
        noexcept
        {
            constexpr auto byte_size = sizeof(T);
            std::array<char, byte_size> bytes{};

            stream.read(bytes.data(), byte_size);

            if (!stream.good())
                return std::nullopt;

            //Put all data in network byteorder (big endian)
            if constexpr (std::endian::native == std::endian::little)
                std::ranges::reverse(bytes);
            //FIXME: watch out for mixed endian!

            T obj{};
            std::memcpy(&obj, bytes.data(), byte_size);

            return obj;
        }

        template <>
        std::optional<Instruction> read<Instruction>(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, data, std::nullopt)

            return Instruction::from_binary(data);
        }

        template <typename T>
        std::optional<std::vector<T>> read_vector(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, vector_size, std::nullopt)

            std::vector<T> obj{};
            obj.reserve(vector_size);

            for (std::uint32_t i = 0; i < vector_size; i++) {
                TRY_READ(T, elem, std::nullopt)
                obj.emplace_back(std::move(elem));
            }

            return obj;
        }

    } // namespace details

    namespace V6 {

        std::optional<std::vector<double>> read_immediate_section(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, immediates_size, std::nullopt);

            std::vector<double> vec{};
            vec.reserve(immediates_size);

            for (std::uint32_t i{}; i != immediates_size; ++i) {
                TRY_READ(double, value, std::nullopt);
                vec.emplace_back(value);
            }

            return vec;
        }

        std::optional<std::vector<VM::CallFrameDescriptor>> read_scope_data(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, scopes_size, std::nullopt);

            std::vector<VM::CallFrameDescriptor> call_frames{};

            for (std::uint32_t i{}; i != scopes_size; ++i) {
                TRY_READ(std::uint8_t, frame_size, std::nullopt);
                if (auto maybe_instructions = details::read_vector<Instruction>(stream); maybe_instructions.has_value()) {
                    call_frames.emplace_back(
                        VM::CallFrameDescriptor{.size = frame_size, .instructions = std::move(maybe_instructions).value()});
                } else
                    return std::nullopt;
            }

            return call_frames;
        }

        ReadResult do_read(std::istream& stream) noexcept
        {
            TRY_READ(std::uint8_t, num_input_constants, ReadingErrorCode::reading_failure);
            TRY_READ(std::uint8_t, num_output_variables, ReadingErrorCode::reading_failure);

            auto maybe_immediates = read_immediate_section(stream);
            if (!maybe_immediates.has_value())
                return ReadingErrorCode::reading_failure;

            auto maybe_scopes = read_scope_data(stream);
            if (!maybe_scopes.has_value())
                return ReadingErrorCode::reading_failure;

            return VM::VMData{
                .num_input_identifiers = num_input_constants,
                .num_output_identifiers = num_output_variables,
                .immediate_values = std::move(maybe_immediates).value(),
                .call_frames = std::move(maybe_scopes).value(),
            };
        }

    } // namespace V6

    ReadResult read_rsbf(std::istream& stream) noexcept
    {
        if (!stream)
            return ReadingErrorCode::file_not_found;

        TRY_READ(std::uint32_t, magic, ReadingErrorCode::reading_failure)
        if (magic != magic_word)
            return ReadingErrorCode::no_magic_word;

        TRY_READ(std::uint32_t, version, ReadingErrorCode::reading_failure)
        if (version > version_number())
            return ReadingErrorCode::wrong_version;

        if (version <= 4)
            Logger::warn("This file was written with RSBF v", version, " which is deprecated due to byteorder changes!\n");

        if (version_number() >= 6 && version <= 4)
            return ReadingErrorCode::wrong_version; //Version 5 is not backwards compatible because of byteorder changes

        if (version != version_number())
            Logger::warn("Mismatched versions between reading library and written file. Please consider regenerating the file\n");

        if (version == 6)
            return V6::do_read(stream);
        return ReadingErrorCode::wrong_version;
    }

} //namespace RaychelScript::Assembly
