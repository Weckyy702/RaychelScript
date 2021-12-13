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

#include "Parser/ParserPipe.h"

namespace RaychelScript::Pipes {

    template <std::floating_point T = double>
    class Interpret
    {
    public:
        explicit Interpret(const Interpreter::ParameterMap<T>& parameters) : parameters_{parameters}
        {}

        Interpreter::ExecutionResult<T> operator()(const AST& ast) const noexcept
        {
            return Interpreter::interpret(ast, parameters_);
        }

    private:
        Interpreter::ParameterMap<T> parameters_;
    };

    template <std::floating_point T>
    Interpreter::ExecutionResult<T> operator|(const Parser::ParseResult& input, const Interpret<T>& interpreter) noexcept
    {
        if (const auto* ec = std::get_if<Parser::ParserErrorCode>(&input); ec) {
            return Interpreter::InterpreterErrorCode::no_input; //TODO: better error handling for this
        }
        return interpreter(Raychel::get<AST>(input));
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_INTERPRETER_PIPE_H