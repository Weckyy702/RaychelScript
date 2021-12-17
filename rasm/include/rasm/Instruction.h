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

#include "InstructionData.h"
#include "OpCode.h"

#include <optional>
#include <ostream>

namespace RaychelScript::Assembly {

    class Instruction
    {
    public:
        explicit Instruction(OpCode code, InstructionData data1 = {}, InstructionData data2 = {})
            : code_{code}, data1_{data1}, data2_{data2}
        {}

        static std::optional<Instruction> from_binary(std::uint32_t data) noexcept
        {
            const auto op_code = static_cast<OpCode>((data >> 24U) & 0xFFU);
            if (op_code >= OpCode::num_op_codes) {
                return std::nullopt;
            }

            InstructionData data1;
            {
                auto index = static_cast<std::uint32_t>((data >> 16U) & 0xFFU);
                if (index & 0x80U) {
                    index &= ~0x80U;
                    if ((index) > static_cast<std::uint32_t>(Register::num_registers)) {
                        return std::nullopt;
                    }
                    data1 = InstructionData{static_cast<Register>(index)};
                } else {
                    data1 = InstructionData{operator ""_mi(index)};
                }
            }

            InstructionData data2;
            {
                const auto index = static_cast<std::uint32_t>((data >> 8U) & 0xFFU);
                if (index & 0x80U) {
                    if ((index & ~0x80U) > static_cast<std::uint32_t>(Register::num_registers)) {
                        return std::nullopt;
                    }
                    data2 = InstructionData{static_cast<Register>(index & ~0x80U)};
                } else {
                    data1 = InstructionData{operator ""_mi(index)};
                }
            }

            return Instruction{op_code, data1, data2};
        }

        std::uint32_t to_binary() const noexcept
        {
            /*
            Instruction layout:
            |........|........|........|........|
             OpCode   DATA1    DATA2    reserved
            */

            std::uint32_t instr = 0U;

            instr |= (static_cast<std::uint32_t>(code_) & 0xFFU) << 24U;
            instr |= (data1_.to_binary() & 0xFFU) << 16U;
            instr |= (data2_.to_binary() & 0xFFU) << 8U;

            return instr;
        }

        friend std::ostream& operator<<(std::ostream& os, const Instruction& instr) noexcept;

    private:
        OpCode code_;
        InstructionData data1_;
        InstructionData data2_;
    };

    inline std::ostream& operator<<(std::ostream& os, const Instruction& instr) noexcept
    {
        auto num_args = number_of_arguments(instr.code_);

        os << instr.code_;
        if (num_args > 0) {
            os << ' ' << instr.data1_;
        }
        if (num_args > 1) {
            os << ' ' << instr.data2_;
        }
        return os;
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H