/**
* \file AssemblerErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssemblerErrorCode class
* \date 2021-12-21
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
#ifndef RAYCHELSCRIPT_ASSEMBLER_ERROR_CODE_H
#define RAYCHELSCRIPT_ASSEMBLER_ERROR_CODE_H

#include <ostream>
#include <string_view>

#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript::Assembler {

    enum class AssemblerErrorCode {
        ok,
        duplicate_name,
        unresolved_identifier,
        unknown_arithmetic_expression,
        not_implemented,
    };

    inline std::string_view error_code_to_reason_string(AssemblerErrorCode ec) noexcept
    {
        switch (ec) {
            case AssemblerErrorCode::ok:
                return "Everything's fine! :)";
            case AssemblerErrorCode::duplicate_name:
                return "Duplicate name";
            case AssemblerErrorCode::unresolved_identifier:
                return "Unresolved identifier";
            case AssemblerErrorCode::unknown_arithmetic_expression:
                return "Unknown arithmetic operator";
            case AssemblerErrorCode::not_implemented:
                return "Not implemented";
        }
        RAYCHEL_ASSERT_NOT_REACHED;
    }

    inline std::ostream& operator<<(std::ostream& os, AssemblerErrorCode ec)
    {
        return os << error_code_to_reason_string(ec);
    }

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLER_ERROR_CODE_H