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

#include <cstdint>
#include <concepts>

#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript::Assembly {

    class MemoryIndex
    {

    public:
        [[nodiscard]] constexpr std::uint8_t value() const noexcept
        {
            return data_;
        }

        constexpr MemoryIndex() = default;

    private:
        template <std::integral T>
        explicit constexpr MemoryIndex(T index) : data_{static_cast<std::uint8_t>(index)}
        {}

        template <std::integral T>
        friend constexpr MemoryIndex make_memory_index(T value);

        std::uint8_t data_{};
    };

    template <std::integral T>
    constexpr MemoryIndex make_memory_index(T value)
    {
        RAYCHEL_ASSERT(value >= 0 && value < T{std::numeric_limits<std::uint8_t>::max()});
        return MemoryIndex{value};
    }

    constexpr MemoryIndex operator""_mi(unsigned long long value) //NOLINT(google-runtime-int): we cannot change the C++ spec :(
    {
        return make_memory_index(value);
    }

    inline std::ostream& operator<<(std::ostream& os, const MemoryIndex& index)
    {
        return os << '$' << static_cast<std::uint32_t>(index.value());
    }

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_RASM_MEMORY_INDEX_H