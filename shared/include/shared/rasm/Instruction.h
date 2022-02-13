/**
* \file Instruction.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Instruction class
* \date 2021-12-13
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H
#define RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H

#include "OpCode.h"
#include "MemoryIndex.h"

#include <optional>
#include <ostream>

namespace RaychelScript::Assembly {

    //TODO: cramming all the instruction data into just 32 bits feels a bit silly. Maybe we should just use 32-bit memory indecies

    class Instruction
    {
    public:
        explicit Instruction(OpCode op_code, MemoryIndex data1 = {}, MemoryIndex data2 = {})
            : code_{op_code}, data1_{data1.value()}, data2_{data2.value()}
        {}

        static std::optional<Instruction> from_binary(std::uint32_t data) noexcept
        {
            const auto op_code = static_cast<OpCode>((data >> 24U) & 0xFFU);
            if (op_code >= OpCode::num_op_codes) {
                return std::nullopt;
            }

            const auto data1 = static_cast<std::uint8_t>((data >> 16U) & 0xFFU);
            const auto data2 = static_cast<std::uint8_t>((data >> 8U) & 0xFFU);

            return Instruction{op_code, make_memory_index(data1), make_memory_index(data2)};
        }

        [[nodiscard]] std::uint32_t to_binary() const noexcept
        {
            /*
            Instruction layout:
            |........|........|........|........|
             OpCode   DATA1    DATA2    zero
            */

            std::uint32_t instr = 0U;

            instr |= (static_cast<std::uint32_t>(code_) & 0xFFU) << 24U;
            instr |= (static_cast<std::uint32_t>(data1_) & 0xFFU) << 16U;
            instr |= (static_cast<std::uint32_t>(data2_) & 0xFFU) << 8U;

            return instr;
        }

        [[nodiscard]] auto op_code() const noexcept
        {
            return code_;
        }

        [[nodiscard]] auto data1() const noexcept
        {
            return data1_;
        }

        [[nodiscard]] auto data2() const noexcept
        {
            return data2_;
        }

        [[nodiscard]] auto& data1() noexcept
        {
            return data1_;
        }

        [[nodiscard]] auto& data2() noexcept
        {
            return data2_;
        }

        friend std::ostream& operator<<(std::ostream& os, const Instruction& instr) noexcept;

    private:
        OpCode code_;
        std::uint8_t data1_;
        std::uint8_t data2_;
    };

    inline std::ostream& operator<<(std::ostream& os, const Instruction& instr) noexcept
    {
        auto num_args = number_of_arguments(instr.code_);

        os << instr.code_;
        if (num_args > 0) {
            os << " $" << static_cast<std::uint32_t>(instr.data1_);
        }
        if (num_args > 1) {
            os << " $" << static_cast<std::uint32_t>(instr.data2_);
        }
        return os;
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H