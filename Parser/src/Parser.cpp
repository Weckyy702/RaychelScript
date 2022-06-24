/**
* \file Parser.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for Parsing functions
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

#include "Parser/Parser.h"

#include <algorithm>
#include <variant>
#include <vector>

using SourceTokens = std::vector<std::vector<RaychelScript::Token>>;
using LineTokens = std::vector<RaychelScript::Token>;

namespace RaychelScript::Parser {

    template <std::size_t N>
    requires(N != 0)
        [[nodiscard]] bool check_token_list(const LineTokens& line, const std::array<Token, N>& expected_tokens) noexcept
    {
        if (line.size() != N) {
            return false;
        }

        for (std::size_t i = 0; i < N; i++) {
            if (line.at(i) != expected_tokens.at(i)) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool check_for_valid_config_header(const LineTokens& line) noexcept
    {
        return check_token_list(
            line,
            std::array{
                Token{TokenType::left_bracket},
                Token{TokenType::left_bracket},
                Token{TokenType::identifer, {}, "config"},
                Token{TokenType::right_bracket},
                Token{TokenType::right_bracket},
            });
    }

    //see BodyBlock.cpp for details
    ParseResult parse_body_block(const SourceTokens& source_tokens, AST ast) noexcept;

    //see ConfigBlock.cpp for details
    [[nodiscard]] bool parse_config_block(const SourceTokens& config_tokens, AST& ast) noexcept;

    [[nodiscard]] auto get_block_tokens(const SourceTokens& source_tokens) noexcept
    {
        const auto& first_line = source_tokens.front();

        if (!check_for_valid_config_header(first_line)) {
            Logger::error(first_line.at(0).location, ": Expected first line to be [[config]]!\n");
            return std::make_pair(SourceTokens{}, SourceTokens{});
        }

        const auto body_block_line = std::find_if(std::next(source_tokens.begin()), source_tokens.end(), [](const auto& tokens) {
            return check_token_list(
                tokens,
                std::array{
                    Token{TokenType::left_bracket},
                    Token{TokenType::left_bracket},
                    Token{TokenType::identifer, {}, "body"},
                    Token{TokenType::right_bracket},
                    Token{TokenType::right_bracket},
                });
        });

        if (body_block_line == source_tokens.end()) {
            Logger::error("Script does not contain [[body]] block!\n");
            return std::make_pair(SourceTokens{}, SourceTokens{});
        }

        auto config_tokens = SourceTokens{std::next(source_tokens.begin()), body_block_line};
        auto body_tokens = SourceTokens{std::next(body_block_line), source_tokens.end()};

        return std::make_pair(std::move(config_tokens), std::move(body_tokens));
    }

    ParseResult parse(const SourceTokens& source_tokens) noexcept
    {
        if (source_tokens.empty()) {
            Logger::error("got empty token list!\n");
            return ParserErrorCode::no_input;
        }

        AST ast;

        const auto [config_tokens, body_tokens] = get_block_tokens(source_tokens);

        if (config_tokens.empty() && body_tokens.empty()) {
            return ParserErrorCode::invalid_config;
        }

        if (!parse_config_block(config_tokens, ast)) {
            return ParserErrorCode::invalid_config;
        }

        if (ast.config_block.input_identifiers.empty()) {
            Logger::error("Invalid config block! Missing input specification!\n");
            return ParserErrorCode::invalid_config;
        }

        if (ast.config_block.output_identifiers.empty()) {
            Logger::error("Invalid config block! Missing output specification!\n");
            return ParserErrorCode::invalid_config;
        }

        return parse_body_block(body_tokens, std::move(ast));
    }

    ParseResult _parse_no_config_check(const SourceTokens& source_tokens) noexcept
    {
        AST ast;
        return parse_body_block(source_tokens, ast);
    }

} // namespace RaychelScript::Parser
