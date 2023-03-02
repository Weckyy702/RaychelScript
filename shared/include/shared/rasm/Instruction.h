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

#include "MemoryIndex.h"
#include "OpCode.h"

#include <optional>
#include <ostream>

namespace RaychelScript::Assembly {

    class Instruction
    {
    public:
        explicit Instruction() = default;

        explicit Instruction(OpCode op_code, MemoryIndex index1 = {}, MemoryIndex index2 = {})
            : code_{op_code}, index1_{index1}, index2_{index2}
        {}

        static std::optional<Instruction> from_binary(std::uint32_t data) noexcept
        {
            const auto code = static_cast<OpCode>((data >> 24U) & 0xFFU);
            if (code >= OpCode::num_op_codes) {
                return std::nullopt;
            }
            const auto maybe_index1 = MemoryIndex::from_binary((data >> 12U) & 0xFFFU);
            if (!maybe_index1.has_value()) {
                return std::nullopt;
            }
            const auto maybe_index2 = MemoryIndex::from_binary(data & 0xFFFU);
            if (!maybe_index2.has_value()) {
                return std::nullopt;
            }

            return Instruction{code, maybe_index1.value(), maybe_index2.value()};
        }

        [[nodiscard]] std::uint32_t to_binary() const noexcept
        {
            /*
            Instruction layout:
            |....:....|....:....|....:....|....:....|
            |OpCode...|....Index1....|....Index2....|
            */

            std::uint32_t instr{};

            instr |= (static_cast<std::uint32_t>(code_) & 0xFFU) << 24U;
            instr |= index1_.to_binary() << 12U;
            instr |= index2_.to_binary();

            return instr;
        }

        [[nodiscard]] auto op_code() const noexcept
        {
            return code_;
        }

        [[nodiscard]] auto index1() const noexcept
        {
            return index1_;
        }

        [[nodiscard]] auto index2() const noexcept
        {
            return index2_;
        }

        [[nodiscard]] auto& index1() noexcept
        {
            return index1_;
        }

        [[nodiscard]] auto& index2() noexcept
        {
            return index2_;
        }

    private:
        OpCode code_{OpCode::num_op_codes};
        MemoryIndex index1_{};
        MemoryIndex index2_{};
    };

    inline std::ostream& operator<<(std::ostream& os, const Instruction& instr) noexcept
    {
        const auto num_args = number_of_arguments(instr.op_code());

        os << instr.op_code();
        if (num_args > 0) {
            os << ' ' << instr.index1();
        }
        if (num_args > 1) {
            os << ' ' << instr.index2();
        }
        return os;
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H
