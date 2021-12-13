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
#include <string_view>

namespace RaychelScript::Assembly {

    enum class OpCode : std::uint8_t {
        mov, //move
        ldi, //load immediate
        add, //add b to a
        sub, //subtract b from a
        mul, //multiply a by b
        div, //divide a by b
        abs, //take the absolute value of a
        fac, //compute the factorial of a
    };

    std::string_view to_mnemonic(OpCode code) noexcept
    {

        switch (code) {
            case OpCode::mov:
                return "MOV";
            case OpCode::ldi:
                return "LDI";
            case OpCode::add:
                return "ADD";
            case OpCode::sub:
                return "SUB";
            case OpCode::mul:
                return "MUL";
            case OpCode::div:
                return "DIV";
            case OpCode::abs:
                return "ABS";
            case OpCode::fac:
                return "FAC";
        }
        return "<unknown>";
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_OPCODE_H