/**
* \file AssemblerErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssemblerErrorCode class
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_ERROR_CODE_H
#define RAYCHELSCRIPT_ASSEMBLY_ERROR_CODE_H

#include <ostream>
#include <string_view>
#include <type_traits>

#define RAYCHELSCRIPT_ASSEMBLY_BEGIN_ASSEMBLER_ERROR_CODE_ENUM                                                                   \
    ok = 0, duplicate_name, unresolved_identifier, unknown_arithmetic_expression, invalid_scope_pop,

namespace RaychelScript::Assembly {

    enum class AssemblerErrorCode {
        RAYCHELSCRIPT_ASSEMBLY_BEGIN_ASSEMBLER_ERROR_CODE_ENUM
    };

    inline std::string_view error_code_to_reason_string(AssemblerErrorCode code) noexcept
    {
        switch (code) {
            case AssemblerErrorCode::ok:
                return "ok";
            case AssemblerErrorCode::duplicate_name:
                return "Duplicate name";
            case AssemblerErrorCode::unresolved_identifier:
                return "Unresolved identifier";
            case AssemblerErrorCode::unknown_arithmetic_expression:
                return "Unknown arithmetic operator";
            case AssemblerErrorCode::invalid_scope_pop:
                return "Tried to pop scope off empty stack";
        }
        return "Unknown error";
    }

    inline std::ostream& operator<<(std::ostream& os, AssemblerErrorCode& code)
    {
        return os << error_code_to_reason_string(code);
    }

    template <typename T>
    requires std::is_enum_v<T> T to_error_code(AssemblerErrorCode code)
    noexcept
    {
        //FIXME: this requires that every assembler error code enum must have
        return static_cast<T>(code);
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_ERROR_CODE_H