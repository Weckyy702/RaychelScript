/**
* \file OpCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for OpCode enum
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_OPCODE_H
#define RAYCHELSCRIPT_ASSEMBLY_OPCODE_H

#include <cstdint>
#include <ostream>
#include <string_view>

#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript::Assembly {

    enum class OpCode : std::uint8_t {
        mov, //move a into b (a -> b)
        add, //add b to a (a = a + b)
        sub, //subtract b from a (a = a - b)
        mul, //multiply a by b (a = a * b)
        div, //divide a by b (a = a / b)
        mag, //take the magnitude of a (a = |a|)
        fac, //compute the factorial of a (a = a!)
        pow, //compute a to the power of b (a = a^b)

        jpz, //jump if a is equal to 0 (a == 0)
        hlt, //halt execution

        num_op_codes
    };

    [[nodiscard]] constexpr std::string_view to_mnemonic(OpCode code) noexcept
    {
        switch (code) {
            case OpCode::mov:
                return "MOV";
            case OpCode::add:
                return "ADD";
            case OpCode::sub:
                return "SUB";
            case OpCode::mul:
                return "MUL";
            case OpCode::div:
                return "DIV";
            case OpCode::mag:
                return "MAG";
            case OpCode::fac:
                return "FAC";
            case OpCode::pow:
                return "POW";
            case OpCode::jpz:
                return "JPZ";
            case OpCode::hlt:
                return "HLT";
            case OpCode::num_op_codes:
                break;
        }
        return "<unknown>";
    }

    [[nodiscard]] constexpr std::size_t number_of_arguments(OpCode code) noexcept
    {
        switch (code) {
            case OpCode::mov:
                return 2;
            case OpCode::jpz:
                return 1;
            case OpCode::add:
            case OpCode::sub:
            case OpCode::mul:
            case OpCode::div:
            case OpCode::pow:
            case OpCode::mag:
            case OpCode::fac:
            case OpCode::hlt:
            case OpCode::num_op_codes:
                return 0;
        }
        RAYCHEL_ASSERT_NOT_REACHED;
    }

    inline std::ostream& operator<<(std::ostream& os, OpCode code) noexcept
    {
        return os << to_mnemonic(code);
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_OPCODE_H