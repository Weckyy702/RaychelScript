/**
* \file LexResult.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for LexResult class
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
#ifndef RAYCHELSCRIPT_LEXRESULT_H
#define RAYCHELSCRIPT_LEXRESULT_H

#include "shared/Lexing/Token.h"

#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

namespace RaychelScript::Lexer {

    enum class LexerErrorCode {
        ok,
        no_input,
        invalid_number,
        invalid_token,
        reserved_identifier,
        unmatched_parenthesis,
    };

    constexpr std::string_view error_code_to_reason_string(LexerErrorCode error_code) noexcept
    {
        switch (error_code) {
            case LexerErrorCode::ok:
                return "No error :)";
            case LexerErrorCode::no_input:
                return "No Input";
            case LexerErrorCode::invalid_number:
                return "Invalid Number";
            case LexerErrorCode::invalid_token:
                return "Invalid Token";
            case LexerErrorCode::reserved_identifier:
                return "Use of Reserved Identifier";
            case LexerErrorCode::unmatched_parenthesis:
                return "Unmatched Parenthesis";
        }
        return "<unknown error>";
    }

    inline std::ostream& operator<<(std::ostream& os, LexerErrorCode ec)
    {
        return os << error_code_to_reason_string(ec);
    }

    using SourceTokens = std::vector<std::vector<Token>>;
    using LexResult = std::variant<LexerErrorCode, SourceTokens>;

} //namespace RaychelScript::Lexer

#endif //!RAYCHELSCRIPT_LEXRESULT_H