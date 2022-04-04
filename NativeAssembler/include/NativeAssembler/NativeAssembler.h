/**
* \file NativeAssembler.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for NativeAssembler class
* \date 2022-04-01
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
#ifndef RAYCHELSCRIPT_NATIVE_ASSEMBLER_H
#define RAYCHELSCRIPT_NATIVE_ASSEMBLER_H

#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include "NativeAssemblerErrorCode.h"
#include "shared/VM/VMData.h"
#define RAYCHELSCRIPT_NATIVE_ASSEMBLER_DECLARE_ASSEMBLER_FUNCTION(tag)                                                           \
    NativeAssemblerErrorCode assemble(const VM::VMData& data, tag##_Tag, std::ostream& output_stream) noexcept;                  \
                                                                                                                                 \
    inline std::variant<NativeAssemblerErrorCode, std::string> assemble_string(const VM::VMData& data, tag##_Tag _tag)           \
    {                                                                                                                            \
        std::stringstream ss;                                                                                                    \
        if (const auto ec = assemble(data, _tag, ss); ec != NativeAssemblerErrorCode::ok) {                                      \
            return ec;                                                                                                           \
        }                                                                                                                        \
        return ss.str();                                                                                                         \
    }

namespace RaychelScript::NativeAssembler {
    inline namespace details {
        struct X86_64_Tag
        {};
    } // namespace details
    constexpr details::X86_64_Tag assemble_x86_64{};

    RAYCHELSCRIPT_NATIVE_ASSEMBLER_DECLARE_ASSEMBLER_FUNCTION(X86_64)

} // namespace RaychelScript::NativeAssembler
#endif //RAYCHELSCRIPT_NATIVE_ASSEMBLER_H
