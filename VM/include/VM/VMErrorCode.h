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
        unknow_op_code,
        not_implemented,
        mismatched_input_identifiers,
        divide_by_zero,
        invalid_argument,
    };

    std::string_view error_code_to_reason_string(VMErrorCode code) noexcept
    {
        switch (code) {
            case VMErrorCode::ok:
                return "Everything's fine :)";
            case VMErrorCode::unknow_op_code:
                return "Unknown instruction op code";
            case VMErrorCode::not_implemented:
                return "Handler not implemented";
            case VMErrorCode::mismatched_input_identifiers:
                return "Number of specified input identifiers does not match number of values provided";
            case VMErrorCode::divide_by_zero:
                return "Division by zero";
            case VMErrorCode::invalid_argument:
                return "Invalid argument to mathematical function";
        }

        return "<unkown>";
    }

    std::ostream& operator<<(std::ostream& os, VMErrorCode code)
    {
        return os << error_code_to_reason_string(code);
    }

} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_ERROR_CODE_H