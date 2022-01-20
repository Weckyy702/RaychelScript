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

#include <array>
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

            if (!stream.good()) {
                return std::nullopt;
            }

            T obj{};
            std::memcpy(&obj, bytes.data(), byte_size);

            return obj;
        }

        template <>
        std::optional<std::string> read<std::string>(std::istream& stream) noexcept
        {
            TRY_READ(std::uint32_t, string_size, std::nullopt)

            std::string obj{};
            obj.reserve(string_size);

            for (std::uint32_t i = 0; i < string_size; i++) {
                TRY_READ(char, next_char, std::nullopt)
                obj.push_back(next_char);
            }

            return obj;
        }

        template <>
        std::optional<MemoryIndex> read<MemoryIndex>(std::istream& stream) noexcept
        {
            TRY_READ(std::uint8_t, value, std::nullopt);
            return make_memory_index(value);
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

        template <typename T1, typename T2>
        std::optional<std::pair<T1, T2>> read_pair(std::istream& stream) noexcept
        {
            TRY_READ(T1, value_1, std::nullopt);
            TRY_READ(T2, value_2, std::nullopt);

            return std::make_pair(value_1, value_2);
        }

    } // namespace details

    std::optional<std::vector<std::pair<double, MemoryIndex>>> read_immediate_section(std::istream& stream) noexcept
    {
        TRY_READ(std::uint32_t, immediates_size, std::nullopt);

        std::vector<std::pair<double, MemoryIndex>> vec{};
        vec.reserve(immediates_size);

        for (std::uint32_t i = 0; i < immediates_size; i++) {
            if (auto maybe_value = details::read_pair<double, MemoryIndex>(stream); maybe_value.has_value()) {
                vec.emplace_back(std::move(maybe_value.value()));
            } else {
                return {};
            }
        }
        return vec;
    }

    ReadResult read_rsbf(std::istream& stream) noexcept
    {
        if (!stream) {
            return ReadingErrorCode::file_not_found;
        }

        TRY_READ(std::uint32_t, magic, ReadingErrorCode::reading_failure)
        if (magic != magic_word) {
            return ReadingErrorCode::no_magic_word;
        }

        TRY_READ(std::uint32_t, version, ReadingErrorCode::reading_failure)
        if (version > version_number()) {
            return ReadingErrorCode::wrong_version;
        }
        if (version != version_number()) {
            Logger::warn("Mismatched versions between reading library and written file. Please consider regenerating the file\n");
        }

        //since we always have the reserved A register, 0 is actually a sentinel value :)
        std::uint8_t number_of_memory_locations{0};

        if (version > 2) {
            TRY_READ(std::uint8_t, num_memory_locations, ReadingErrorCode::reading_failure);
            number_of_memory_locations = num_memory_locations;
        }

        std::size_t identifier_index{1};
        std::vector<std::pair<std::string, MemoryIndex>> input_identifiers{};

        if (version > 3) {
            TRY_READ(std::uint32_t, size, ReadingErrorCode::reading_failure);
            for (std::uint32_t i = 0; i < size; i++) {
                if (auto maybe_value = details::read_pair<std::string, MemoryIndex>(stream); maybe_value.has_value()) {
                    input_identifiers.emplace_back(std::move(maybe_value.value()));
                }
            }
        } else {
            auto maybe_input_identifiers = details::read_vector<std::string>(stream);
            if (!maybe_input_identifiers.has_value()) {
                return ReadingErrorCode::reading_failure;
            }

            //we have to convert the old data format to the new one (pain)
            for (auto& identifier : maybe_input_identifiers.value()) {
                input_identifiers.emplace_back(std::move(identifier), make_memory_index(identifier_index++));
            }
        }

        std::vector<std::pair<std::string, MemoryIndex>> output_identifiers{};

        if (version > 3) {
            TRY_READ(std::uint32_t, size, ReadingErrorCode::reading_failure);
            for (std::uint32_t i = 0; i < size; i++) {
                if (auto maybe_value = details::read_pair<std::string, MemoryIndex>(stream); maybe_value.has_value()) {
                    output_identifiers.emplace_back(std::move(maybe_value.value()));
                }
            }
        } else {

            auto maybe_output_identifiers = details::read_vector<std::string>(stream);
            if (!maybe_output_identifiers.has_value()) {
                return ReadingErrorCode::reading_failure;
            }

            //we have to convert the old data format to the new one (pain)
            for (auto& identifier : maybe_output_identifiers.value()) {
                output_identifiers.emplace_back(std::move(identifier), make_memory_index(identifier_index++));
            }
        }

        //The immediate section was added in version 2
        std::optional<std::vector<std::pair<double, MemoryIndex>>> maybe_immediates{};
        if (version > 1) {
            if (auto _maybe_immediates = read_immediate_section(stream); _maybe_immediates.has_value()) {
                maybe_immediates = std::move(_maybe_immediates).value();
            }
        }

        TRY_READ(std::uint32_t, num_instructions, ReadingErrorCode::reading_failure)
        std::vector<Instruction> instructions;
        instructions.reserve(num_instructions);

        for (std::size_t i = 0; i < num_instructions; i++) {
            auto maybe_instruction = details::read<Instruction>(stream);
            if (!maybe_instruction.has_value()) {
                return ReadingErrorCode::reading_failure;
            }
            instructions.emplace_back(maybe_instruction.value());
        }

        return VMData{
            .config_block = {input_identifiers, output_identifiers},
            .immediate_values = maybe_immediates.value_or(std::vector<std::pair<double, MemoryIndex>>{}),
            .instructions = instructions,
            .num_memory_locations = number_of_memory_locations};
    }

} //namespace RaychelScript::Assembly
