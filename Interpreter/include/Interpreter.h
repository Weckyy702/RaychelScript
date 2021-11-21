/**
* \file Interpreter.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Interpreter functions
* \date 2021-11-15
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
#ifndef RAYCHELSCRIPT_INTERPRETER_H
#define RAYCHELSCRIPT_INTERPRETER_H

#include <concepts>
#include <map>
#include <string>
#include <variant>

#include "AST/AST.h"
#include "ExecutionErrorCode.h"
#include "ExecutionState.h"
#include "Parser.h"

#include "RaychelCore/AssertingGet.h"

namespace RaychelScript::Interpreter {

    template <std::floating_point T>
    using ExecutionResult = std::variant<InterpreterErrorCode, ExecutionState<T>>;

    template <std::floating_point T>
    using ParameterMap = std::map<std::string, T>;

    [[nodiscard]] ExecutionResult<double> interpret(const AST& ast, const ParameterMap<double>& parameters) noexcept;

    [[nodiscard]] inline ExecutionResult<double>
    interpret(std::istream& source_stream, const ParameterMap<double>& parameters) noexcept
    {
        const auto ast_or_error = Parser::parse(source_stream);
        if (const auto* ec = std::get_if<Parser::ParserErrorCode>(&ast_or_error); ec) {
            return InterpreterErrorCode::no_input;
        }

        return interpret(Raychel::get<AST>(ast_or_error), parameters);
    }

    [[nodiscard]] inline ExecutionResult<double>
    interpret(const std::string& source_text, const ParameterMap<double>& parameters) noexcept
    {
        std::stringstream stream{source_text};
        return interpret(stream, parameters);
    }

} //namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_INTERPRETER_H