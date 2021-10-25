/**
* \file BodyBlock.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Source file for parsing functions regarding the [[body]] block of the program. Just for Project structure
* \date 2021-10-19
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

#include <array>
#include <charconv>
#include <ranges>
#include <variant>
#include <vector>

#include "IndentHandler.h"
#include "NodeData.h"
#include "Parser.h"
#include "Token.h"

#include "RaychelCore/AssertingGet.h"
#include "RaychelCore/ClassMacros.h"

using LineTokens = std::vector<RaychelScript::Token>;
using LineView = std::ranges::subrange<LineTokens::const_iterator>;
using SourceTokens = std::vector<LineTokens>;

namespace RaychelScript {

    //TODO: clean this mess up
    template <std::size_t N>
    requires(N != 0) SourceTokens match_token_pattern(LineView tokens, const std::array<TokenType::TokenType, N>& pattern)
    noexcept
    {
        if (tokens.size() < N) {
            return {}; //The token list must be at least as long as the pattern
        }

        auto pattern_it = pattern.begin();
        auto token_it = tokens.begin();

        SourceTokens matches;

        while (pattern_it != pattern.end() && token_it != tokens.end()) {
            const auto& current = *token_it;
            const auto& expected = *pattern_it;

            auto& match = matches.emplace_back();

            if (expected == TokenType::expression_) {
                if (pattern_it == std::prev(pattern.end())) {
                    for (auto it = token_it; it != tokens.end(); it++) {
                        match.emplace_back(*it);
                        token_it++;
                    }
                    pattern_it++;
                    break;
                }

                const auto escape_token = *std::next(pattern_it);
                for (auto it = token_it; it != tokens.end(); it++) {
                    if (it->type == escape_token) {
                        pattern_it++;
                        break;
                    }
                    token_it++;
                    match.emplace_back(*it);
                }
            } else if (current.type == expected) {
                match.emplace_back(current);
                pattern_it++;
                token_it++;
            } else {
                return {};
            }
        }

        if (token_it != tokens.end() || pattern_it != pattern.end()) {
            return {}; //If we didn't handle all tokens, the pattern did not match
        }

        return matches;
    }

    /**
    * \brief Check if an operator token falls into the MD part of PEMDAS
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] static bool is_MD_op(const Token& token) noexcept
    {
        const auto& type = token.type;

        return (type == TokenType::star) || (type == TokenType::slash);
    }

    /**
    * \brief Check if an operator token falls into the AS part of PEMDAS
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] static bool is_AS_op(const Token& token) noexcept
    {
        const auto& type = token.type;

        return (type == TokenType::plus) || (type == TokenType::minus);
    }

    /**
    * \brief Check if a token is an arithmetic operator
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] static bool is_arith_op(const Token& token) noexcept
    {
        return is_MD_op(token) || is_AS_op(token);
    }

    /**
    * \brief Find the least precedent arithmetic operator, if any
    * 
    * \param tokens List of tokens to scan
    * \return Iterator to the least precedent token, tokens.end() if no operator was found
    * 
    * TODO: this doesn't handle parentheses yet
    */
    [[nodiscard]] static LineTokens::const_iterator find_arithmetic_operator(LineView tokens) noexcept
    {
        if (tokens.empty()) {
            return tokens.end();
        }

        auto it = tokens.end();
        bool op_was_MD = false;

        auto current = std::prev(tokens.end());
        do {
            if (!is_arith_op(*current)) {
                if (current->type != TokenType::identifer && current->type != TokenType::number) {
                    break; //We can break early if the expression contains tokens not allowed in arithmetic expressions
                }
                continue;
            }
            if (is_AS_op(*current)) {
                it = current;
                break; //If we found an AS operator, we can exit early
            }
            if (!op_was_MD) {
                it = current;
                op_was_MD = true;
            }
        } while (current-- != tokens.begin());

        return it;
    }

    [[nodiscard]] inline std::string_view get_op_string_from_token_type(TokenType::TokenType type) noexcept
    {
        using namespace std::string_view_literals;
        switch (type) {
            case TokenType::star:
                return "multiplication"sv;
            case TokenType::slash:
                return "division"sv;
            case TokenType::plus:
                return "addition"sv;
            case TokenType::minus:
                return "subtraction"sv;
            default:
                RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    [[nodiscard]] static ArithmeticExpressionData::Operation get_op_type_from_token_type(TokenType::TokenType type) noexcept
    {
        using Op = ArithmeticExpressionData::Operation;
        switch (type) {
            case TokenType::star:
                return Op::multiply;
            case TokenType::slash:
                return Op::divide;
            case TokenType::plus:
                return Op::add;
            case TokenType::minus:
                return Op::subtract;
            default:
                RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    template <NodeData T>
    [[nodiscard]] static std::variant<ParserErrorCode, AST_Node>
    handle_two_component_expression(LineView lhs, LineView rhs) noexcept;

    [[nodiscard]] static std::variant<ParserErrorCode, AST_Node>
    handle_math_op(LineView lhs, LineView rhs, ArithmeticExpressionData::Operation op) noexcept;

    [[nodiscard]] static std::variant<ParserErrorCode, AST_Node> parse_expression(LineView expression_tokens) noexcept
    {
        using namespace TokenType;
        using std::array;

        IndentHandler handler;

        if (expression_tokens.empty()) {
            Logger::error(handler.indent(), "parse_expression got empty token list!\n");
            return ParserErrorCode::no_input;
        }

        //Assignment expressions
        if (const auto matches = match_token_pattern(expression_tokens, array{expression_, equal, expression_});
            !matches.empty()) {
            Logger::debug(handler.indent(), "Found assignment expression at ", matches.at(1).front().location, '\n');

            return handle_two_component_expression<AssignmentExpressionData>(matches.at(0), matches.at(2));
        }

        //Unary operators
        if (const auto matches = match_token_pattern(expression_tokens, array{minus, expression_}); !matches.empty()) {
            Logger::debug(handler.indent(), "Found unary minus expression at ", matches.front().front().location, '\n');
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{plus, expression_}); !matches.empty()) {
            Logger::debug(handler.indent(), "Found unary plus expression at ", matches.front().front().location, '\n');
        }

        //Mathematical operators
        if (const auto op_it = find_arithmetic_operator(expression_tokens); op_it != expression_tokens.end()) {

            Logger::debug(
                handler.indent(), "Found ", get_op_string_from_token_type(op_it->type), " expression at ", op_it->location, '\n');

            const auto op = get_op_type_from_token_type(op_it->type);
            const auto lhs = LineView{expression_tokens.begin(), op_it};
            const auto rhs = LineView{std::next(op_it), expression_tokens.end()};

            return handle_math_op(lhs, rhs, op);
        }

        //TODO: operator-assign constructs would be here (*= += -= /= etc.)

        //Logical operators

        //Misc

        //Leaf nodes
        if (const auto matches = match_token_pattern(expression_tokens, array{number}); !matches.empty()) {
            Logger::debug(
                handler.indent(),
                "Found numeric constant at ",
                matches.front().front().location,
                " value=",
                *matches.front().front().content,
                '\n');

            const auto& token = matches.front().front();

            const std::string_view value_str = *token.content;

            double value{0};
            if (const auto [_, ec] = std::from_chars(std::begin(value_str), std::end(value_str), value); ec != std::errc{}) {
                Logger::error(handler.indent(), "Unable to interpret string '", value_str, "' as a double!");
                return ParserErrorCode::invalid_numeric_constant;
            }

            return AST_Node{NumericConstantData{{}, value}};
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{identifer}); !matches.empty()) {
            Logger::debug(
                handler.indent(),
                "Found variable reference at ",
                matches.front().front().location,
                " name=",
                *matches.front().front().content,
                '\n');

            const auto& token = matches.front().front();

            auto name = *token.content;

            //TODO: handle non-existent variable names

            return AST_Node{VariableReferenceData{{}, std::move(name)}};
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{declaration, identifer}); !matches.empty()) {
            Logger::debug(
                handler.indent(),
                "Found variable declaration at ",
                matches.front().front().location,
                " name=",
                *matches.front().front().content,
                '\n');

            auto name = *matches.at(1).front().content;
            const bool is_const = matches.front().front().content == "let";

            //TODO: handle duplicate declarations

            return AST_Node{VariableDeclarationData{{}, std::move(name), is_const}};
        }

        Logger::error(handler.indent(), "Unknown construct at ", expression_tokens.begin()->location, "!\n");
        return ParserErrorCode::invalid_construct;
    }

    template <NodeData T>
    [[nodiscard]] static std::variant<ParserErrorCode, AST_Node>
    handle_two_component_expression(LineView lhs_tokens, LineView rhs_tokens) noexcept
    {
        const auto lhs_node_or_error = parse_expression(lhs_tokens);
        const auto rhs_node_or_error = parse_expression(rhs_tokens);

        if (const auto* ec = std::get_if<ParserErrorCode>(&lhs_node_or_error); ec) {
            return *ec;
        }
        if (const auto* ec = std::get_if<ParserErrorCode>(&rhs_node_or_error); ec) {
            return *ec;
        }

        auto lhs_node = Raychel::get<AST_Node>(lhs_node_or_error);
        auto rhs_node = Raychel::get<AST_Node>(rhs_node_or_error);

        return AST_Node{T{{}, lhs_node, rhs_node}};
    }

    [[nodiscard]] static std::variant<ParserErrorCode, AST_Node>
    handle_math_op(LineView lhs_tokens, LineView rhs_tokens, ArithmeticExpressionData::Operation op) noexcept
    {
        const auto lhs_node_or_error = parse_expression(lhs_tokens);
        const auto rhs_node_or_error = parse_expression(rhs_tokens);

        if (const auto* ec = std::get_if<ParserErrorCode>(&lhs_node_or_error); ec) {
            return *ec;
        }
        if (const auto* ec = std::get_if<ParserErrorCode>(&rhs_node_or_error); ec) {
            return *ec;
        }

        auto lhs_node = Raychel::get<AST_Node>(lhs_node_or_error);
        auto rhs_node = Raychel::get<AST_Node>(rhs_node_or_error);

        return AST_Node{ArithmeticExpressionData{{}, lhs_node, rhs_node, op}};
    }

    static ParserErrorCode parse_body_line(LineView line, AST& ast) noexcept
    {
        auto node_or_error = parse_expression(line);
        if (const auto* ec = std::get_if<ParserErrorCode>(&node_or_error); ec) {
            return *ec;
        }

        ast.nodes.push_back(std::move(Raychel::get<AST_Node>(node_or_error)));
        return {};
    }

    std::variant<AST, ParserErrorCode> parse_body_block(const SourceTokens& source_tokens, AST& ast) noexcept
    {
        ParserErrorCode ec{};
        std::for_each(source_tokens.begin(), source_tokens.end(), [&](const auto& line) {
            if (ec == ParserErrorCode{}) {
                ec = parse_body_line(line, ast);
            }
        });

        if (ec != ParserErrorCode{}) {
            return ec;
        }
        return ast;
    }

    std::size_t IndentHandler::indent_ = 0; //I don't know where to put this

} // namespace RaychelScript
