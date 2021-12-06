/**
* \file ExecutionErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for execution error codes
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
#ifndef RAYCHELSCRIPT_EXECUTION_ERROR_CODE_H
#define RAYCHELSCRIPT_EXECUTION_ERROR_CODE_H

#include <string_view>

namespace RaychelScript::Interpreter {

    enum class [[nodiscard]] InterpreterErrorCode{
        ok,
        no_input,
        not_enough_input_identifiers,
        invalid_input_identifier,
        divide_by_zero,
        constant_reassign,
        invalid_argument,

        invalid_node,
        duplicate_name,
        unresolved_identifier,
        invalid_arithmetic_operation,
        invalid_relational_operation,
        pop_empy_stack,
    };

    [[nodiscard]] inline std::string_view error_code_to_reason_string(InterpreterErrorCode ec) noexcept
    {
        using namespace std::string_view_literals;
        using EC = InterpreterErrorCode;
        switch (ec) {
            //General error codes
            case EC::ok:
                return "Everything's fine :)"sv;
            case EC::no_input:
                return "No Input"sv;
            case EC::not_enough_input_identifiers:
                return "Not enough input identifiers were provided"sv;
            case EC::invalid_input_identifier:
                return "Input identifier was not in the ASTs input specification";
            case EC::divide_by_zero:
                return "Division by Zero"sv;
            case EC::constant_reassign:
                return "Attempt to reassign to constant value";
            case EC::invalid_node:
                return "Encountered unknown AST node"sv;
            case EC::duplicate_name:
                return "Dupliate identifier"sv;
            case EC::unresolved_identifier:
                return "Unable to resolve identifier to an existing descriptor"sv;
            case EC::invalid_arithmetic_operation:
                return "Unknown arithmetic operation in arithmetic expression node"sv;
            case EC::invalid_argument:
                return "Invalid argument to function"sv;
            case EC::invalid_relational_operation:
                return "Unknown relational operator in relational expression node"sv;
            case EC::pop_empy_stack:
                return "Attempt to pop empty scope stack"sv;
        }
        return "<Unknown Reason>"sv;
    }

} // namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_EXECUTION_ERROR_CODE_H