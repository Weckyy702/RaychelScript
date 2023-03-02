/**
* \file MemoryIndex.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for MemoryIndex class
* \date 2022-02-13
*
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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
#ifndef RAYCHELSCRIPT_RASM_MEMORY_INDEX_H
#define RAYCHELSCRIPT_RASM_MEMORY_INDEX_H

#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript::Assembly {

    class MemoryIndex
    {
    public:
        enum class ValueType : std::uint8_t {
            immediate = 1,
            stack,
            intermediate,
            jump_offset,
            num_value_types,
        };

    private:
        template <std::integral T>
        explicit constexpr MemoryIndex(T index, ValueType type) : data_{static_cast<std::uint8_t>(index)}, type_{type}
        {}

    public:
        using enum ValueType;

        static_assert(static_cast<std::uint8_t>(num_value_types) - 1 <= 0xFU, "Memory index value types must fit into 4 bytes");

        [[nodiscard]] constexpr std::uint8_t value() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr ValueType type() const noexcept
        {
            return type_;
        }

        [[nodiscard]] static constexpr std::optional<MemoryIndex> from_binary(std::uint32_t data) noexcept
        {
            const auto value = static_cast<std::uint8_t>(data >> 4U);
            const auto type = static_cast<ValueType>(data & 0xFU);
            if (type >= ValueType::num_value_types)
                return std::nullopt;
            return MemoryIndex{value, type};
        }

        [[nodiscard]] constexpr std::uint32_t to_binary() const noexcept
        {
            //Index layout:
            //|....:....|....:....|....:....|....:....|
            //|..........Zero..........|..Data...|Type|
            return (static_cast<std::uint32_t>(data_) << 4U) | static_cast<std::uint32_t>(type_);
        }

        constexpr MemoryIndex() = default;

        constexpr auto operator<=>(const MemoryIndex& other) const = default;

    private:
        template <std::integral T>
        friend constexpr MemoryIndex make_memory_index(T value, ValueType type);

        std::uint8_t data_{};
        ValueType type_{};
    };

    template <std::integral T>
    constexpr auto operator<=>(MemoryIndex a, T b)
    {
        return a.value() <=> b;
    }

    template <std::integral T>
    constexpr MemoryIndex make_memory_index(T value, MemoryIndex::ValueType type)
    {
        if (type == MemoryIndex::ValueType::jump_offset) {
            RAYCHEL_ASSERT(std::in_range<std::int8_t>(value)); //jump offsets can be negative
        } else {
            RAYCHEL_ASSERT(std::in_range<std::uint8_t>(value));
        }
        return MemoryIndex{value, type};
    }

    constexpr MemoryIndex operator""_mi(unsigned long long value) //NOLINT(google-runtime-int): we cannot change the C++ spec :(
    {
        return make_memory_index(value, MemoryIndex::ValueType::stack);
    }

    constexpr MemoryIndex operator""_imm(unsigned long long value) //NOLINT(google-runtime-int): we cannot change the C++ spec :(
    {
        return make_memory_index(value, MemoryIndex::ValueType::immediate);
    }

    constexpr char prefix_for(MemoryIndex::ValueType type)
    {
        using enum MemoryIndex::ValueType;
        switch (type) {
            case immediate:
                return '%';
            case stack:
                return '$';
            case intermediate:
                return '&';
            case jump_offset:
                return '~';
            case num_value_types:
                break;
        }
        return '!';
    }

    inline std::ostream& operator<<(std::ostream& os, const MemoryIndex& index)
    {
        os << prefix_for(index.type());
        if (index.type() == MemoryIndex::ValueType::jump_offset)
            return os << static_cast<std::int32_t>(static_cast<std::int8_t>(index.value()));
        return os << static_cast<std::uint32_t>(static_cast<std::uint8_t>(index.value()));
    }

} // namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_RASM_MEMORY_INDEX_H
