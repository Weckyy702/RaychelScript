/**
* \file Lexer.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation of Lexing functions
* \date 2021-09-29
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

#include "Lexer.h"

#include <RaychelLogger/Logger.h>
#include "Lexing/Alphabet.h"

namespace RaychelScript::Lexer {

    TokenType::TokenType parse_token(const std::string& token) noexcept
    {
        //if the token starts with a digit, it must be a number
        if (is_number_char(token.front())) {
            return TokenType::number;
        }
        if (token == "let" || token == "var") {
            return TokenType::declaration;
        }
        return TokenType::identifer;
    }

    //TODO: make this code less awful
    std::optional<std::vector<Token>> _lex_raw(std::istream& source_stream) noexcept
    {
        std::vector<Token> tokens{};
        std::string current_token{};

        bool in_comment = false;
        bool might_be_number = false;

        std::size_t line_number = 1U;
        std::size_t column_number = 1U;

        int line_paren_depth{0};

        bool has_failed = false;

        const auto reset_state = [&] {
            if (!current_token.empty()) {
                auto type = parse_token(current_token);
                tokens.emplace_back(type, SourceLocation{line_number, column_number}, current_token);
                current_token.clear();
            }
            might_be_number = false;
        };

        const auto is_part_of_digit = [&current_token](char c) {
            const bool is_digit = is_number_char(c);
            const bool is_dot_in_number = c == '.' && is_number_char(current_token.back());
            const bool number_already_has_dot = current_token.find('.') != std::string::npos;

            return is_digit || (is_dot_in_number && !number_already_has_dot);
        };

        const auto handle_regular_char = [&](char c) {
            if (is_identifier_char(c) && !might_be_number) {
                might_be_number = false;
            } else if (is_part_of_digit(c)) {
                might_be_number = true;
            } else {
                Logger::error(line_number, ':', column_number, " : invalid Token!\n");
                return false;
            }
            current_token.push_back(c);
            return true;
        };

        const auto label = Logger::startTimer("lex_time");

        for (char c{0}; source_stream.get(c) && !has_failed;) {
            column_number++;

            if (c == '\n') {
                reset_state();
                if (tokens.empty() || tokens.back().type != TokenType::newline) { //multiple newlines in sequence are ignored
                    tokens.emplace_back(TokenType::newline, SourceLocation{line_number, column_number});
                }

                if (line_paren_depth != 0) {
                    Logger::error("Unmatched parenthesis in line ", line_number, "!\n");
                    has_failed = true;
                }

                line_paren_depth = 0;
                in_comment = false;
                column_number = 1U;
                line_number++;
            }

            if (in_comment) {
                continue;
            }

            if (is_opening_parenthesis(static_cast<TokenType::TokenType>(c))) {
                line_paren_depth++;
            } else if (is_closing_parenthesis(static_cast<TokenType::TokenType>(c))) {
                line_paren_depth--;
            }

            if (c == '#') {
                in_comment = true;
            } else if (std::isspace(c) != 0) {
                reset_state();
            } else if (is_special_char(c)) {
                reset_state();
                tokens.emplace_back(static_cast<TokenType::TokenType>(c), SourceLocation{line_number, column_number});
            } else {
                if (!handle_regular_char(c)) {
                    return {};
                }
            }
        }
        reset_state();

        const auto dur = Logger::getTimer(label);
        Logger::debug("Lexing ", line_number, " line(s) took ", dur.count(), " ms\n");

        return tokens;
    }

    std::optional<std::vector<std::vector<Token>>>
    combine_tokens_into_lines(const std::optional<std::vector<Token>>& _raw_tokens) noexcept
    {
        if (!_raw_tokens.has_value()) {
            return {};
        }

        const auto& raw_tokens = *_raw_tokens;
        std::vector<std::vector<Token>> tokens;
        std::vector<Token> current_line;

        for (const auto& current_token : raw_tokens) {
            if (current_token.type == TokenType::newline) {
                if (!current_line.empty()) {
                    tokens.push_back(current_line);
                    current_line.clear();
                }
                continue;
            }

            current_line.push_back(current_token);
        }
        if (!current_line.empty()) {
            tokens.push_back(current_line);
            current_line.clear();
        }

        return tokens;
    }

} //namespace RaychelScript