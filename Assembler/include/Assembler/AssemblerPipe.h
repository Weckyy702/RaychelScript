/**
* \file AssemblerPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssemblerPipe class
* \date 2021-12-19
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
#ifndef RAYCHELSCRIPT_PIPES_ASSEMBLER_H
#define RAYCHELSCRIPT_PIPES_ASSEMBLER_H

#include "Assembler.h"

#include "shared/Pipes/PipeResult.h"

namespace RaychelScript::Pipes {

    struct Assemble
    {
        auto operator()(const AST& ast)
        {
            return Assembler::assemble(ast);
        }
    };

    inline PipeResult<VM::VMData> operator|(const PipeResult<AST>& input, Assemble assembler) noexcept
    {
        RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(input);
        return assembler(input.value());
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_PIPES_ASSEMBL_H