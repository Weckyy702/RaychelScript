/**
* \file NativeAssemblerErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for NativeAssemblerErrorCode class
* \date 2022-04-02
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
#ifndef RAYCHELSCRIPT_NATIVE_ASSEMBLER_ERROR_CODE_H
#define RAYCHELSCRIPT_NATIVE_ASSEMBLER_ERROR_CODE_H

#include <iostream>
#include <string_view>

namespace RaychelScript::NativeAssembler {

    enum class NativeAssemblerErrorCode {
        ok = 0,
        unknown_instruction,
        stream_write_error,
    };

    constexpr std::string_view error_code_to_reason_string(NativeAssemblerErrorCode ec)
    {
        using N = NativeAssemblerErrorCode;
        switch (ec) {
            case N::ok:
                return "Everything's fine :)";
            case N::unknown_instruction:
                return "Unknown instruction in RASM bytecode";
            case N::stream_write_error:
                return "Error while writing to output stream";
        }
        return "Unknown error code!";
    }

    inline std::ostream& operator<<(std::ostream& os, NativeAssemblerErrorCode ec)
    {
        return os << error_code_to_reason_string(ec);
    }

} //namespace RaychelScript::NativeAssembler

#endif //!RAYCHELSCRIPT_NATIVE_ASSEMBLER_ERROR_CODE_H
