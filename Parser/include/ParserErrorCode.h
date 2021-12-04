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

    enum class [[nodiscard]] ParserErrorCode : std::size_t{
        ok,
        no_input,

        //Configuration block error codes
        invalid_config,

        //Construct error codes
        invalid_construct,
        invalid_declaration,
        invalid_numeric_constant,
        mismatched_conditional,

        //Semantic analysis error codes
        assign_to_non_value_ref,
        assign_rhs_not_number_type,
        arith_op_not_number_type,
        op_assign_lhs_not_identifier,
        unary_op_rhs_not_number_type,
        conditional_construct_condition_not_boolean_type,
        relational_op_lhs_not_number_type,
        relational_op_rhs_not_number_type,
    };

    constexpr std::string_view error_code_to_reason_string(ParserErrorCode ec) noexcept
    {
        using namespace std::string_view_literals;
        using RaychelScript::Parser::ParserErrorCode;

        switch (ec) {
            case ParserErrorCode::ok:
                return "Everything's fine :)"sv;
            case ParserErrorCode::no_input:
                return "No Input"sv;
            case ParserErrorCode::invalid_config:
                return "Error in configuration block"sv;
            case ParserErrorCode::invalid_construct:
                return "Invalid construct"sv;
            case ParserErrorCode::invalid_declaration:
                return "Invalid variable declaration"sv;
            case ParserErrorCode::invalid_numeric_constant:
                return "Invalid numeric constant"sv;
            case ParserErrorCode::mismatched_conditional:
                return "Mismatched if/endif"sv;
            case ParserErrorCode::assign_to_non_value_ref:
                return "Trying to assign to something that is not a value reference"sv;
            case ParserErrorCode::assign_rhs_not_number_type:
                return "Right-hand side of assignment expressio does not have 'number' type"sv;
            case ParserErrorCode::arith_op_not_number_type:
                return "Operand in arithmetic expression does not have 'number' type"sv;
            case ParserErrorCode::op_assign_lhs_not_identifier:
                return "Left-hand side of operator-assign expression is not an identifier"sv;
            case ParserErrorCode::unary_op_rhs_not_number_type:
                return "Operand of unary operator does not have 'number' type"sv;
            case ParserErrorCode::conditional_construct_condition_not_boolean_type:
                return "Condition expression of conditional construct does not have 'boolean' type"sv;
            case ParserErrorCode::relational_op_lhs_not_number_type:
                return "Left-hand side of relational operator does not have 'number' type"sv;
            case ParserErrorCode::relational_op_rhs_not_number_type:
                return "Left-hand side of relational operator does not have 'number' type"sv;
        }
        return "<Unknown reason>"sv;
    }

    inline std::ostream& operator<<(std::ostream& os, ParserErrorCode obj) noexcept
    {
        return os << error_code_to_reason_string(obj);
    }

} //namespace RaychelScript::Parser

#endif //!RAYCHELSCRIPT_PARSER_ERROR_CODE_H