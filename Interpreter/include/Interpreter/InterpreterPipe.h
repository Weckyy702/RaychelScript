/**
* \file InterpreterPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Interpreter pipe API
* \date 2021-12-04
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
#ifndef RAYCHELSCRIPT_INTERPRETER_PIPE_H
#define RAYCHELSCRIPT_INTERPRETER_PIPE_H

#include "Interpreter.h"

#include <map>

#include "shared/Pipes/PipeResult.h"

namespace RaychelScript::Pipes {

    class Interpret
    {
    public:
        explicit Interpret(std::map<std::string, double> parameters) : parameters_{std::move(parameters)}
        {}

        Interpreter::ExecutionResult operator()(const AST& ast) const noexcept
        {
            return Interpreter::interpret(ast, parameters_);
        }

    private:
        std::map<std::string, double> parameters_;
    };

    inline PipeResult<Interpreter::State> operator|(const PipeResult<AST>& input, const Interpret& interpreter) noexcept
    {
        RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(input);
        return interpreter(input.value());
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_INTERPRETER_PIPE_H
