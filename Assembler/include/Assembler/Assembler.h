/**
* \file Assembler.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Assembler functions
* \date 2021-12-18
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_H
#define RAYCHELSCRIPT_ASSEMBLY_H

#include <variant>

#include "AssemblerErrorCode.h"

#include "shared/AST/AST.h"
#include "shared/VM/VMData.h"

#ifdef _WIN32
    #ifdef RaychelScriptAssembler_EXPORTS
        #define RAYCHELSCRIPT_ASSEMBLER_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_ASSEMBLER_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_ASSEMBLER_API
#endif

namespace RaychelScript::Assembler {

    RAYCHELSCRIPT_ASSEMBLER_API [[nodiscard]] std::variant<AssemblerErrorCode, VM::VMData> assemble(const AST& ast) noexcept;

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLY_H
