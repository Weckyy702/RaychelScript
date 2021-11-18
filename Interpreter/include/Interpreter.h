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
#include "Execution/ExecutionErrorCode.h"
#include "Execution/ExecutionState.h"

namespace RaychelScript::Interpreter {

    template <std::floating_point T>
    using ExecutionResult = std::variant<ExecutionErrorCode, ExecutionState<T>>;

    [[nodiscard]] ExecutionResult<double>
    interpret(const AST& ast, const std::map<std::string, double>& input_identifiers) noexcept;

} //namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_INTERPRETER_H