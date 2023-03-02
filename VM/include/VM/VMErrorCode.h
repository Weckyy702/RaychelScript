/**
* \file VMErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VMErrorCode enum
* \date 2021-12-27
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
#ifndef RAYCHELSCRIPT_VM_ERROR_CODE_H
#define RAYCHELSCRIPT_VM_ERROR_CODE_H

#include <ostream>
#include <string_view>

namespace RaychelScript::VM {

    enum class VMErrorCode {
        ok,
        not_implemented,
        mismatched_inputs,
        mismatched_outputs,
        stack_overflow,
        stack_underflow,
        unknown_opcode,
        divide_by_zero,
        invalid_operand,
        memory_overflow,
    };

    inline std::string_view error_code_to_reason_string(VMErrorCode code) noexcept
    {
        using enum VMErrorCode;
        switch (code) {
            case ok:
                return "Everything's fine :)";
            case not_implemented:
                return "Not implemented";
            case mismatched_inputs:
                return "Number of provided inputs does not match programs inputs";
            case mismatched_outputs:
                return "Number of provided output slots does not match programs outputs";
            case stack_overflow:
                return "Call stack overflow";
            case stack_underflow:
                return "Call stack underflow";
            case unknown_opcode:
                return "Unknown instruction op code";
            case divide_by_zero:
                return "Divide by zero";
            case invalid_operand:
                return "Invalid oprand to arithmetic operation";
            case memory_overflow:
                return "Memory Overflow";
        }

        return "<unkown>";
    }

    inline std::ostream& operator<<(std::ostream& os, VMErrorCode code)
    {
        return os << error_code_to_reason_string(code);
    }

} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_ERROR_CODE_H
