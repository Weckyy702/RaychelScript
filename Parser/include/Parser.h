/**
* \file Parser.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for RaychelScript parsing functions
* \date 2021-10-02
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
#ifndef RAYCHELSCRIPT_PARSER_H
#define RAYCHELSCRIPT_PARSER_H

#include <istream>
#include <variant>
#include <vector>
#include "AST.h"
#include "Lexer.h"

namespace RaychelScript {

    enum class [[nodiscard]] ParserErrorCode : std::size_t{
        no_input = 1,
        invalid_config,
        invalid_construct,
        invalid_declaration,
        invalid_numeric_constant,
    };

    std::variant<AST, ParserErrorCode> parse(const std::vector<std::vector<Token>>& source_tokens) noexcept;

    inline std::variant<AST, ParserErrorCode> parse(std::istream& source_stream) noexcept
    {
        const auto tokens = lex(source_stream);
        if (!tokens.has_value()) {
            return ParserErrorCode::no_input;
        }
        return parse(*tokens);
    }

    inline std::variant<AST, ParserErrorCode> parse(const std::string& source_text) noexcept
    {
        std::stringstream s{source_text};
        return parse(s);
    }


    /**
    * \brief Parse the source tokens without checking for a valid config block
    * 
    * This function is intended for live-parsing from stdin.
    * 
    * \warning THIS FUNCTION DOES NOT PRODUCE A VALID AST!
    * 
    * \param source_tokens 
    * \return  
    */
    std::variant<AST, ParserErrorCode> _parse_no_config_check(const std::vector<std::vector<Token>>& source_tokens) noexcept;

    /**
    * \brief Parse the source text without checking for a valid config block
    * 
    * This function is intended for live-parsing from stdin.
    * 
    * \warning THIS FUNCTION DOES NOT PRODUCE A VALID AST!
    * 
    * \param source_text
    * \return  
    */
    inline std::variant<AST, ParserErrorCode> _parse_no_config_check(const std::string& source_text) noexcept
    {
        const auto maybe_tokens = lex(source_text);

        if(!maybe_tokens.has_value()) {
            return ParserErrorCode::no_input;
        }

        return _parse_no_config_check(*maybe_tokens);
    }
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_PARSER_H