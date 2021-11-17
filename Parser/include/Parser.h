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

#ifdef _WIN32
    #ifdef RaychelScriptParser_EXPORTS
        #define RAYCHELSCRIPT_PARSER_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_PARSER_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_PARSER_API
#endif

#include <istream>
#include <string_view>
#include <variant>
#include <vector>
#include "AST/AST.h"
#include "Lexer.h"

namespace RaychelScript::Parser {

    enum class [[nodiscard]] ParserErrorCode : std::size_t {
        ok,
        no_input,
        invalid_config,
        invalid_construct,
        invalid_declaration,
        invalid_numeric_constant,
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
        }
        return "<Unknown reason>"sv;
    }

    RAYCHELSCRIPT_PARSER_API std::variant<AST, ParserErrorCode>
    parse(const std::vector<std::vector<Token>>& source_tokens) noexcept;

    inline std::variant<AST, ParserErrorCode> parse(std::istream& source_stream) noexcept
    {
        const auto tokens = Lexer::lex(source_stream);
        if (!tokens.has_value()) {
            return ParserErrorCode::no_input;
        }
        return parse(*tokens);
    }

    inline std::variant<AST, ParserErrorCode> parse(const std::string& source_text) noexcept
    {
        std::stringstream stream{source_text};
        return parse(stream);
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
    RAYCHELSCRIPT_PARSER_API std::variant<AST, ParserErrorCode>
    _parse_no_config_check(const std::vector<std::vector<Token>>& source_tokens) noexcept;

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
        const auto maybe_tokens = Lexer::lex(source_text);

        if (!maybe_tokens.has_value()) {
            return ParserErrorCode::no_input;
        }

        return _parse_no_config_check(*maybe_tokens);
    }
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_PARSER_H