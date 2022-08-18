/**
* \file ParserErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ParserErrorCode enum
* \date 2021-11-30
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
#ifndef RAYCHELSCRIPT_PARSER_ERROR_CODE_H
#define RAYCHELSCRIPT_PARSER_ERROR_CODE_H

#include <ostream>
#include <string_view>

namespace RaychelScript::Parser {

    enum class ParserErrorCode {
        ok,
        no_input,

        //Configuration block error codes
        invalid_config,

        //Construct error codes
        invalid_construct,
        invalid_declaration,
        invalid_numeric_constant,
        mismatched_conditional,
        mismatched_else,
        mismatched_loop,
        mismatched_header_footer_type,
        invalid_function_argument_list,
        mismatched_endfn,
        duplicate_function,
        invalid_function_definition,
        return_in_invalid_scope,

        //Semantic analysis error codes
        assign_to_non_value_ref,
        assign_rhs_not_number_type,
        arith_op_not_number_type,
        op_assign_lhs_not_identifier,
        unary_op_rhs_not_number_type,
        conditional_construct_condition_not_boolean_type,
        relational_op_lhs_not_number_type,
        relational_op_rhs_not_number_type,
        loop_condition_not_boolean_type,
        simple_function_value_not_number_type,
        function_argument_not_number_type,
        return_expression_not_number_type,
        missing_return,
    };

    constexpr std::string_view error_code_to_reason_string(ParserErrorCode ec) noexcept
    {
        using namespace std::string_view_literals;
        using enum ParserErrorCode; //<3

        switch (ec) {
            case ok:
                return "Everything's fine :)"sv;
            case no_input:
                return "No Input"sv;
            case invalid_config:
                return "Error in configuration block"sv;
            case invalid_construct:
                return "Invalid construct"sv;
            case invalid_declaration:
                return "Invalid variable declaration"sv;
            case invalid_numeric_constant:
                return "Invalid numeric constant"sv;
            case mismatched_conditional:
                return "Mismatched if/endif"sv;
            case mismatched_else:
                return "Invalid else construct"sv;
            case invalid_function_argument_list:
                return "Invalid function argument list"sv;
            case mismatched_endfn:
                return "Mismatched fn/endfn"sv;
            case duplicate_function:
                return "Dupliate function definition"sv;
            case invalid_function_definition:
                return "Function definition at non-global scope"sv;
            case return_in_invalid_scope:
                return "Return statement at non-function scope"sv;
            case assign_to_non_value_ref:
                return "Trying to assign to something that is not a value reference"sv;
            case assign_rhs_not_number_type:
                return "Right-hand side of assignment expressio does not have 'number' type"sv;
            case arith_op_not_number_type:
                return "Operand in arithmetic expression does not have 'number' type"sv;
            case op_assign_lhs_not_identifier:
                return "Left-hand side of operator-assign expression is not an identifier"sv;
            case unary_op_rhs_not_number_type:
                return "Operand of unary operator does not have 'number' type"sv;
            case conditional_construct_condition_not_boolean_type:
                return "Condition expression of conditional construct does not have 'boolean' type"sv;
            case relational_op_lhs_not_number_type:
                return "Left-hand side of relational operator does not have 'number' type"sv;
            case relational_op_rhs_not_number_type:
                return "Right-hand side of relational operator does not have 'number' type"sv;
            case loop_condition_not_boolean_type:
                return "Condition of loop does not have 'boolean' type"sv;
            case mismatched_loop:
                return "Mismatched while/endwhile"sv;
            case mismatched_header_footer_type:
                return "Type of construct header does not match type of construct footer"sv;
            case simple_function_value_not_number_type:
                return "Value expression of simple function definition does not have 'number' type"sv;
            case function_argument_not_number_type:
                return "Argument expression of function call does not have 'number' type"sv;
            case return_expression_not_number_type:
                return "Subexpression of return statement does not have 'number' type"sv;
            case missing_return:
                return "Function does not end with a return statement"sv;
        }
        return "<Unknown reason>"sv;
    }

    inline std::ostream& operator<<(std::ostream& os, ParserErrorCode obj) noexcept
    {
        return os << error_code_to_reason_string(obj);
    }

} //namespace RaychelScript::Parser

#endif //!RAYCHELSCRIPT_PARSER_ERROR_CODE_H
