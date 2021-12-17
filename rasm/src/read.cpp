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
        out_name = maybe_value.value();                                                                                          \
    }

namespace RaychelScript::Assembly {

    namespace details {

        template <typename T>
        std::optional<T> read(std::istream& stream) noexcept;

        template <std::integral T>
        std::optional<T> read(std::istream& stream) noexcept
        {
            constexpr auto byte_size = sizeof(T);
            char byte_data[byte_size]{};

            stream.read(byte_data, byte_size);

            if (!stream.good()) {
                return std::nullopt;
            }

            T obj{};
            std::memcpy(&obj, byte_data, byte_size);

            return obj;
        }

        template <>
        std::optional<std::string> read<std::string>(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, string_size, std::nullopt)

            std::string obj{};
            obj.reserve(string_size);

            for (std::uint32_t i = 0; i < string_size; i++) {
                TRY_READ(char, c, std::nullopt)
                obj.push_back(c);
            }

            return obj;
        }

        template<>
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

            for(std::uint32_t i = 0; i <  vector_size; i++) {
                TRY_READ(T, t, std::nullopt)
                obj.emplace_back(std::move(t));
            }

            return obj;
        }

    } // namespace details

    std::variant<VMData, ReadingErrorCode> read_rsbf(std::istream& stream) noexcept
    {
        if (!stream) {
            return ReadingErrorCode::file_not_found;
        }

        {
            TRY_READ(std::uint32_t, magic, ReadingErrorCode::reading_failure)
            if (magic != magic_word) {
                return ReadingErrorCode::no_magic_word;
            }
        }

        {
            TRY_READ(std::uint32_t, version, ReadingErrorCode::reading_failure)
            if (version > version_number()) {
                return ReadingErrorCode::wrong_version;
            }
        }

        auto maybe_input_identifiers = details::read_vector<std::string>(stream);
        if(!maybe_input_identifiers.has_value()) {
            return ReadingErrorCode::reading_failure;
        }

        auto maybe_output_identifiers = details::read_vector<std::string>(stream);
        if(!maybe_output_identifiers.has_value()) {
            return ReadingErrorCode::reading_failure;
        }

        TRY_READ(std::uint32_t, num_instructions, ReadingErrorCode::reading_failure)
        std::vector<Instruction> instructions;
        instructions.reserve(num_instructions);

        for(std::size_t i = 0; i < num_instructions; i++) {
            auto maybe_instruction = details::read<Instruction>(stream);
            if(!maybe_instruction.has_value()) {
                return ReadingErrorCode::reading_failure;
            }
            instructions.emplace_back(maybe_instruction.value());
        }

        return VMData{{maybe_input_identifiers.value(), maybe_output_identifiers.value(), {}}, instructions};
    }

} //namespace RaychelScript::Assembly
