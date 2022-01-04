/**
* \file TokenType.hpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief TokenyType enum declaration
* \date 2021-09-28
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
#ifndef RAYCHELSCRIPT_TOKEN_TYPE_H
#define RAYCHELSCRIPT_TOKEN_TYPE_H

#include <ostream>
#include <string>
#include "RaychelCore/Raychel_assert.h"

namespace RaychelScript {
    namespace TokenType {

        enum TokenType : char {
            newline = '\n',
            left_paren = '(',
            right_paren = ')',
            left_bracket = '[',
            right_bracket = ']',
            left_curly = '{',
            right_curly = '}',
            comma = ',',
            number = '0',
            identifer = '1',
            declaration = '2',
            conditional_header = '3',
            conditional_footer = '4',
            loop_header = '5',
            loop_footer = '6',
            literal_true = '7',
            literal_false = '8',
            plus = '+',
            minus = '-',
            star = '*',
            slash = '/',
            percent = '%',
            equal = '=',
            left_angle = '<',
            right_angle = '>',
            bang = '!',
            ampersand = '&',
            pipe = '|',
            caret = '^',

            expression_ = '\0',
            arith_op_ = '\1',
        };

    } //namespace TokenType

    inline std::string token_type_to_string(TokenType::TokenType type) noexcept
    {
        using TT = RaychelScript::TokenType::TokenType;

        switch (type) {
            case TT::number:
                return "NUMBER";
            case TT::declaration:
                return "DECLARATION";
            case TT::identifer:
                return "IDENTIFIER";
            case TT::conditional_header:
                return "IF";
            case TT::conditional_footer:
                return "ENDIF";
            case TT::loop_header:
                return "WHILE";
            case TT::loop_footer:
                return "ENDWHILE";
            case TT::literal_true:
                return "TRUE";
            case TT::literal_false:
                return "FALSE";
            case TT::expression_:
            case TT::arith_op_:
                RAYCHEL_TERMINATE("TokenType::expression_ and TokenType::arith_op_ cannot appear in this context!\n");
            default:
                return std::string{type};
        }
    }

    inline std::ostream& operator<<(std::ostream& os, TokenType::TokenType type) noexcept
    {
        return os << token_type_to_string(type);
    }
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_TOKEN_TYPE_H