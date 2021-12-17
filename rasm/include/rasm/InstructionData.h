/**
* \file InstructionData.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for InstructionData class
* \date 2021-12-14
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_DATA_H
#define RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_DATA_H

#include <cstdint>
#include <limits>
#include <ostream>

#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript::Assembly {

    enum class Register : std::uint8_t {
        a,
        b,
        ip,

        num_registers,
    };

    class MemoryIndex
    {

    public:
        [[nodiscard]] auto value() const noexcept
        {
            return index_;
        }

    private:
        constexpr explicit MemoryIndex(std::uint8_t value) : index_{value}
        {}

        friend constexpr MemoryIndex operator""_mi(unsigned long long index); //NOLINT(google-runtime-int): the standard forced my hand

        std::uint8_t index_{};
    };

    constexpr MemoryIndex operator""_mi(unsigned long long index) //NOLINT(google-runtime-int): the standard forced my hand
    {
        RAYCHEL_ASSERT(index < 0x80ULL);
        return MemoryIndex{static_cast<std::uint8_t>(index)};
    }

    class InstructionData
    {
    public:
        InstructionData() = default;

        /*implicit*/ InstructionData(MemoryIndex memory_index) : data_{memory_index.value()}
        {}

        /*implicit*/ InstructionData(Register _register) : is_register_{true}, data_{static_cast<std::uint8_t>(_register)}
        {
            //Accessing out of bounds registers would lead to mayhem
            RAYCHEL_ASSERT(_register < Register::num_registers);
        }

        [[nodiscard]] std::uint32_t to_binary() const noexcept
        {
            //register indecies have the highest bit set
            if (is_register_) {
                return data_ | 0x80U;
            }
            return data_;
        }

        friend std::ostream& operator<<(std::ostream& os, const InstructionData& data) noexcept;

    private:
        bool is_register_{false};
        std::uint32_t data_{0};
    };

    inline std::ostream& operator<<(std::ostream& os, const InstructionData& data) noexcept
    {
        return os << (data.is_register_ ? 'R' : '$') << data.data_;
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_DATA_H