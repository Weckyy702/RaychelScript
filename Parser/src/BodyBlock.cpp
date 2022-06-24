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

#include "RaychelCore/compat.h"

#include <array>
#include <charconv>
#if __has_include(<ranges>) && RAYCHEL_ACTIVE_COMPILER != RAYCHEL_COMPILER_CLANG
    #include <ranges>
#else
    #pragma message("Unable to use ranges header. Using a workaround that is a little less elegant")
    #define RAYCHELSCRIPT_NO_RANGES_HEADER 1
#endif
#include <algorithm>
#include <variant>
#include <vector>

#include "Parser/Parser.h"
#include "Parser/ParsingContext.h"
#include "shared/AST/NodeData.h"
#include "shared/IndentHandler.h"
#include "shared/Lexing/Alphabet.h"
#include "shared/Lexing/Token.h"

#include "RaychelCore/AssertingGet.h"
#include "RaychelCore/ClassMacros.h"
#include "RaychelCore/charconv.h"

#define RAYCHELSCRIPT_PARSER_NOISY 1
#define RAYCHELSCRIPT_PARSER_SILENT 1

#if RAYCHELSCRIPT_PARSER_SILENT
    #define RAYCHELSCRIPT_PARSER_DEBUG(...)
#else
    #define RAYCHELSCRIPT_PARSER_DEBUG(...) Logger::debug(__VA_ARGS__, '\n')
#endif

#define TRY_GET_INTERNAL(name, parser_call)                                                                                      \
    auto name##_node_or_error = parser_call;                                                                                     \
    if (const auto* ec = std::get_if<ParserErrorCode>(&(name##_node_or_error)); ec) {                                            \
        return *ec;                                                                                                              \
    }                                                                                                                            \
    auto name##_node = Raychel::get<AST_Node>(std::move((name##_node_or_error)));

#define TRY_GET_CHILD_NODE(name) TRY_GET_INTERNAL(name, parse_statement_or_expression(name##_tokens, ctx))
#define TRY_GET_SUBEXPRESSION(name) TRY_GET_INTERNAL(name, parse_expression(name##_tokens))

using LineTokens = std::vector<RaychelScript::Token>;
#ifndef RAYCHELSCRIPT_NO_RANGES_HEADER
using LineView = std::ranges::subrange<LineTokens::const_iterator>;
#else
using LineView = const LineTokens&;
#endif
using SourceTokens = std::vector<LineTokens>;

using ParseExpressionResult = std::variant<RaychelScript::Parser::ParserErrorCode, RaychelScript::AST_Node>;

namespace RaychelScript::Parser {

    template <typename TokenIt, typename PatternIt>
    [[nodiscard]] static bool handle_subexpression(
        TokenIt& token_it, const TokenIt& token_end, PatternIt& pattern_it, const PatternIt& pattern_end, LineTokens& match,
        int& paren_depth)
    {
        if (pattern_it == std::prev(pattern_end)) {
            for (auto it = token_it; it != token_end; it++) {
                match.emplace_back(*it);
                token_it++;
            }
            pattern_it++;
            return true;
        }

        const auto escape_token = *std::next(pattern_it);
        for (auto it = token_it; it != token_end; it++) {
            if (is_opening_parenthesis(it->type)) {
                paren_depth++;
            } else if (is_closing_parenthesis(it->type)) {
                paren_depth--;
            }
            if (paren_depth == 0 && it->type == escape_token) {
                pattern_it++;
                break;
            }
            token_it++;
            match.emplace_back(*it);
        }
        return paren_depth == 0;
    }

    /**
    * \brief Match a token against a provided pattern.
    *
    * \tparam N size of the pattern.
    *
    *  The purpose of this function is to provide regex-like matching of token sequences.
    *  TokenType::expression_ is used like a wildcard and will mach all tokens up to the next token in the pattern.
    *  TokenType::arith_op_ is used to find all arithmetic operators as defined by is_arith_op().
    */
    template <std::size_t N>
    requires(N != 0) [[nodiscard]] static SourceTokens
        match_token_pattern(LineView tokens, const std::array<TokenType::TokenType, N>& pattern) noexcept
    {
        const auto number_of_subexpressions = static_cast<std::size_t>(std::count_if(
            pattern.begin(), pattern.end(), [](TokenType::TokenType type) { return type == TokenType::expression_; }));

        if (tokens.size() < N - number_of_subexpressions) {
            return {}; //The token list must be at least as long as the pattern minus possibly empty subexpressions
        }

        if (number_of_subexpressions == 0 && tokens.size() != N) {
            return {}; //If there are no subexpressions and there are more tokens than in the pattern, it cannot match
        }

        const auto token_matched = [](TokenType::TokenType type, TokenType::TokenType expected) {
            return (expected == TokenType::arith_op_ && is_arith_op(type)) || (type == expected);
        };

        auto pattern_it = pattern.begin();
        auto token_it = tokens.begin();

        int paren_depth{};

        SourceTokens matches;

        while (pattern_it != pattern.end() && token_it != tokens.end()) {
            const auto& current = *token_it;
            const auto& expected = *pattern_it;

            auto& match = matches.emplace_back();

            if (expected == TokenType::expression_) {
                if (!handle_subexpression(token_it, tokens.end(), pattern_it, pattern.end(), match, paren_depth)) {
                    return {};
                }
                continue;
            }

            if (is_opening_parenthesis(current.type))
                ++paren_depth;
            else if (is_closing_parenthesis(current.type))
                --paren_depth;

            if (token_matched(current.type, expected)) {
                match.emplace_back(current);
                pattern_it++;
                token_it++;
                continue;
            }
            return {};
        }

        if (token_it != tokens.end() || pattern_it != pattern.end()) {
            return {}; //If we didn't handle all tokens, the pattern did not match
        }

        return matches;
    }

    /**
    * \brief Find the least precedent arithmetic operator, if any.
    * 
    * This function finds the operator that will be evaluated last according to operator precedence.
    * Since the AST is inverted, the last operator is the top-level node in the chain
    * 
    * \param tokens List of tokens to scan
    * \return Iterator to the least precedent token, tokens.end() if no operator was found
    */
    [[nodiscard]] static LineTokens::const_iterator find_arithmetic_operator(LineView tokens) noexcept
    {
        //FIXME: handle unary operators
        if (tokens.empty()) {
            return tokens.end();
        }

        int paren_depth{0}; //keep track of how deep we are inside nested parentheses

        auto it = tokens.end();
        bool op_was_MD = false;

        auto current = tokens.end();
        do {

            current--;

            if (!is_arith_op(current->type)) {
                if (is_opening_parenthesis(current->type)) {
                    paren_depth++;
                } else if (is_closing_parenthesis(current->type)) {
                    paren_depth--;
                } else if (!is_allowed_token(current->type)) {
                    return tokens.end();
                }
                continue;
            }

            if (paren_depth != 0) {
                continue;
            }

            if (is_AS_op(current->type)) {
                it = current;
                break; //If we found the right-most AS operator, we can exit early
            }
            if (!op_was_MD) {
                it = current;
                op_was_MD = true;
            }
        } while (current != tokens.begin());

        if (paren_depth != 0) {
            Logger::error("Unmatched parenthesis at ", tokens.back().location, '\n');
            return tokens.end();
        }

        return it;
    }

    [[nodiscard]] bool is_toplevel_parenthesised_expression(LineView tokens)
    {
        if (!is_opening_parenthesis(tokens.front().type)) {
            return false;
        }

        if (!is_closing_parenthesis(tokens.back().type)) {
            return false;
        }

        int paren_depth{0};
        Token const* closing_paren = nullptr;

        std::for_each(tokens.begin(), tokens.end(), [&](const Token& token) {
            if (closing_paren != nullptr) {
                return;
            }

            if (is_opening_parenthesis(token.type)) {
                paren_depth++;
            } else if (is_closing_parenthesis(token.type)) {
                paren_depth--;
                if (paren_depth == 0) {
                    closing_paren = &token;
                }
            }
        });

        return closing_paren == &tokens.back();
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
            case TokenType::caret:
                return "power"sv;
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
            case TokenType::caret:
                return Op::power;
            default:
                RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    [[nodiscard]] static ParseExpressionResult handle_assignment_expression(LineView lhs, LineView rhs) noexcept;

    [[nodiscard]] static ParseExpressionResult
    handle_math_op(LineView lhs, LineView rhs, ArithmeticExpressionData::Operation op) noexcept;

    [[nodiscard]] static ParseExpressionResult
    handle_update_expression(LineView identifier_tokens, LineView rhs, ArithmeticExpressionData::Operation op) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_unary_epression(LineView rhs, UnaryExpressionData::Operation op) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_conditional_header(LineView condition_tokens, ParsingContext& ctx) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_conditional_else(ParsingContext& ctx) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_conditional_footer(ParsingContext& ctx) noexcept;

    [[nodiscard]] static ParseExpressionResult
    handle_relational_operator(LineView lhs, LineView rhs, RelationalOperatorData::Operation op) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_loop_header(LineView condition, ParsingContext& ctx) noexcept;

    [[nodiscard]] static ParseExpressionResult handle_loop_footer(ParsingContext& ctx) noexcept;

    [[nodiscard]] static ParseExpressionResult
    handle_number(const Token& number_token, bool is_negative, ParsingContext& ctx) noexcept;


    [[nodiscard]] static ParseExpressionResult
    parse_statement_or_expression(LineView expression_tokens, ParsingContext& ctx) noexcept
    {
        namespace TT = TokenType;
        using std::array;

        IndentHandler handler;

//With this option enabled, the parsing step will log every expression. Very noisy
#if !RAYCHELSCRIPT_PARSER_SILENT && RAYCHELSCRIPT_PARSER_NOISY
        Logger::debug(handler.indent());
        for (const auto& token : expression_tokens) {
            Logger::log(token_type_to_string(token.type), ' ');
        }
        Logger::log('\n');

#endif

        if (expression_tokens.empty()) {
            Logger::error(handler.indent(), "parse_statement_or_expression got empty token list!\n");
            return ParserErrorCode::no_input;
        }

        //Parenthesised expressions
        if (is_toplevel_parenthesised_expression(expression_tokens)) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found parenthesised expression at ", expression_tokens.front().location);

            return parse_expression(LineView{std::next(expression_tokens.begin()), std::prev(expression_tokens.end())});
        }
        }

        //conditional header
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::conditional_header, TT::expression_});
            !matches.empty()) {

            RAYCHELSCRIPT_PARSER_DEBUG(handler.indent(), "Found conditional header at ", matches.front().front().location);
            return handle_conditional_header(matches.at(1), ctx);
        }

        //else
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::conditional_else}); !matches.empty()) {
            return handle_conditional_else(ctx);
        }

        //conditional footer
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::conditional_footer}); !matches.empty()) {
            return handle_conditional_footer(ctx);
        }

        //loop header
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::loop_header, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(handler.indent(), "Found loop header at ", matches.front().front().location);
            return handle_loop_header(matches.at(1), ctx);
        }

        //loop footer
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::loop_footer}); !matches.empty()) {
            return handle_loop_footer(ctx);
        }

        //Logical operators
        if (const auto matches =
                match_token_pattern(expression_tokens, array{TT::expression_, TT::equal, TT::equal, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG("Found equals expression at ", matches.at(1).front().location);
            return handle_relational_operator(matches.front(), matches.back(), RelationalOperatorData::Operation::equals);
        }

        if (const auto matches =
                match_token_pattern(expression_tokens, array{TT::expression_, TT::bang, TT::equal, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG("Found not-equals expression at ", matches.at(1).front().location);
            return handle_relational_operator(matches.front(), matches.back(), RelationalOperatorData::Operation::not_equals);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::expression_, TT::left_angle, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG("Found less-than expression at ", matches.at(1).front().location);
            return handle_relational_operator(matches.front(), matches.back(), RelationalOperatorData::Operation::less_than);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::expression_, TT::right_angle, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG("Found greater-than expression at ", matches.at(1).front().location);
            return handle_relational_operator(
                matches.front(), matches.back(), RelationalOperatorData::Operation::greater_than, ctx);
        }

        //Operator-assign expressions
        if (const auto matches =
                match_token_pattern(expression_tokens, array{TT::identifer, TT::arith_op_, TT::equal, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(handler.indent(), "Found update expression at ", matches.front().front().location);
            return handle_update_expression(
                matches.front(), matches.back(), get_op_type_from_token_type(matches.at(1).front().type));
        }

        //Assignment expressions
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::expression_, TT::equal, TT::expression_});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(handler.indent(), "Found assignment expression at ", matches.at(1).front().location);

            return handle_assignment_expression(matches.at(0), matches.at(2));
        }

        //Misc

        //Silly trick to parse -NUMBER as a negative number
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::minus, TT::number}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found negative number expression at ", matches.front().front().location);

            return handle_number(matches.at(1).front(), true, ctx);
        }

        //Unary operators
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::minus, TT::expression_}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(handler.indent(), "Found unary minus expression at ", matches.front().front().location);

            return handle_unary_epression(matches.at(1), UnaryExpressionData::Operation::minus);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::plus, TT::expression_}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found unary factorial expression at ", matches.front().front().location);

            return handle_unary_epression(matches.at(1), UnaryExpressionData::Operation::plus);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::expression_, TT::bang}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found unary factorial expression at ", matches.front().front().location);

            return handle_unary_epression(matches.front(), UnaryExpressionData::Operation::factorial);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::pipe, TT::expression_, TT::pipe});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found unary magnitude expression at ", matches.front().front().location);

            return handle_unary_epression(matches.at(1), UnaryExpressionData::Operation::magnitude);
        }

        //Arithmetic operators
        if (const auto op_it = find_arithmetic_operator(expression_tokens); op_it != expression_tokens.end()) {

            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(), "Found ", get_op_string_from_token_type(op_it->type), " expression at ", op_it->location);

            const auto op = get_op_type_from_token_type(op_it->type);
            const auto lhs = LineView{expression_tokens.begin(), op_it};
            const auto rhs = LineView{std::next(op_it), expression_tokens.end()};

            return handle_math_op(lhs, rhs, op);
        }

        //Leaf nodes
        if (const auto matches = match_token_pattern(expression_tokens, array{TT::number}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(),
                "Found numeric constant at ",
                matches.front().front().location,
                " value=",
                *matches.front().front().content);

            return handle_number(matches.front().front(), false, ctx);
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::identifer}); !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(),
                "Found variable reference at ",
                matches.front().front().location,
                " name=",
                *matches.front().front().content);

            const auto& token = matches.front().front();

            auto name = *token.content;

            return AST_Node{VariableReferenceData{{}, std::move(name)}};
        }

        if (const auto matches = match_token_pattern(expression_tokens, array{TT::declaration, TT::identifer});
            !matches.empty()) {
            RAYCHELSCRIPT_PARSER_DEBUG(
                handler.indent(),
                "Found variable declaration at ",
                matches.front().front().location,
                " name=",
                *matches.at(1).front().content);

            auto name = *matches.at(1).front().content;
            const bool is_const = matches.front().front().content == "let";

            return AST_Node{VariableDeclarationData{{}, std::move(name), is_const}};
        }

        Logger::error(handler.indent(), "Unknown construct at ", expression_tokens.begin()->location, '\n');
        return ParserErrorCode::invalid_construct;
    }

    [[nodiscard]] static ParseExpressionResult parse_expression(LineView expression_tokens) noexcept
    {
        std::map<std::string, FunctionData> function_sink{};
        std::vector<AST_Node> node_sink{};
        ParsingContext dummy{function_sink};
        dummy.scopes.push(Scope{ScopeType::global, node_sink});
        auto node_or_error = parse_statement_or_expression(expression_tokens, dummy);

        if (!function_sink.empty()) {
            Logger::error(expression_tokens.at(0).location, ": Function declarations are not allowed in an expression!\n");
            return ParserErrorCode::invalid_construct;
        }
        if (!node_sink.empty()) {
            Logger::error(expression_tokens.front().location, ": Statements (if/loop) are not allowed in an expression!\n");
            return ParserErrorCode::invalid_construct;
        }
        if (const auto* maybe_node = std::get_if<AST_Node>(&node_or_error); maybe_node) {
            if (maybe_node->value_type() == ValueType::none) {
                Logger::error(
                    expression_tokens.front().location, ": Constructs returning 'none' are not allowed in an expression!\n");
                return ParserErrorCode::invalid_construct;
            }
        }
        return node_or_error;
    }

    [[nodiscard]] static ParseExpressionResult handle_assignment_expression(LineView lhs_tokens, LineView rhs_tokens) noexcept
    {
        TRY_GET_SUBEXPRESSION(lhs);
        TRY_GET_SUBEXPRESSION(rhs);

        if (!lhs_node.is_lvalue()) {
            Logger::error("Trying to assign to non-value reference!\n");
            return ParserErrorCode::assign_to_non_value_ref;
        }

        if (rhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Right-hand side of assignment expression does not have value type 'number', has '",
                value_type_to_string(rhs_node.value_type()),
                "' instead!\n");
            return ParserErrorCode::assign_rhs_not_number_type;
        }

        return AST_Node{AssignmentExpressionData{{}, lhs_node, rhs_node}};
    }

    [[nodiscard]] static ParseExpressionResult
    handle_math_op(LineView lhs_tokens, LineView rhs_tokens, ArithmeticExpressionData::Operation op) noexcept
    {
        TRY_GET_SUBEXPRESSION(lhs);
        TRY_GET_SUBEXPRESSION(rhs);

        if (lhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Left-hand side of arithmetic operator does not have value type 'number', has '",
                lhs_node.value_type(),
                "' instead!\n");
            return ParserErrorCode::arith_op_not_number_type;
        }

        if (rhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Right-hand side of arithmetic operator does not have value type 'number', has '",
                rhs_node.value_type(),
                "' instead!\n");
            return ParserErrorCode::arith_op_not_number_type;
        }

        return AST_Node{ArithmeticExpressionData{{}, lhs_node, rhs_node, op}};
    }

    [[nodiscard]] static ParseExpressionResult
    handle_update_expression(LineView identifier_tokens, LineView rhs_tokens, ArithmeticExpressionData::Operation op) noexcept
    {
        TRY_GET_SUBEXPRESSION(identifier);

        if (identifier_node.type() != NodeType::variable_ref) {
            Logger::error("Left-hand side of operator-assign expression is not an identifier!\n");
            return ParserErrorCode::op_assign_lhs_not_identifier;
        }

        TRY_GET_SUBEXPRESSION(rhs);

        if (rhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Right-hand-side of update expression does nat have 'number' type, has '", rhs_node.value_type(), "' instead!\n");
        }

        return AST_Node{UpdateExpressionData{{}, identifier_node, rhs_node, op}};
    }

    [[nodiscard]] static ParseExpressionResult
    handle_unary_epression(LineView rhs_tokens, UnaryExpressionData::Operation op) noexcept
    {
        TRY_GET_SUBEXPRESSION(rhs);

        if (rhs_node.value_type() != ValueType::number) {
            Logger::error("Operand of unary operator does not have 'number' type, has '", rhs_node.value_type(), "' instead!\n");
            return ParserErrorCode::unary_op_rhs_not_number_type;
        }

        return AST_Node{UnaryExpressionData{{}, rhs_node, op}};
    }

    [[nodiscard]] static ParseExpressionResult handle_conditional_header(LineView condition_tokens, ParsingContext& ctx) noexcept
    {
        TRY_GET_SUBEXPRESSION(condition);

        if (condition_node.value_type() != ValueType::boolean) {
            Logger::error(
                "Condition expression of conditional construct does not have 'boolean' type, has '",
                condition_node.value_type(),
                "' instead!\n");
            return ParserErrorCode::conditional_construct_condition_not_boolean_type;
        }

        AST_Node node{ConditionalConstructData{{}, std::move(condition_node)}};

        ctx.conditionals.push(node.to_ref<ConditionalConstructData>());
        ctx.scopes.push({ScopeType::conditional, node.to_ref<ConditionalConstructData>().body});
        ctx.new_scope_started = true;

        return node;
    }

    [[nodiscard]] static ParseExpressionResult handle_conditional_else(ParsingContext& ctx) noexcept
    {
        if (ctx.scopes.top().type != ScopeType::conditional) {
            Logger::error("'else' keyword can only appear after 'if' construct!\n");
            return ParserErrorCode::mismatched_else;
        }

        ctx.scopes.top() = Scope{ScopeType::conditional, ctx.conditionals.top().get().else_body};

        return ParserErrorCode::ok;
    }

    [[nodiscard]] static ParseExpressionResult handle_conditional_footer(ParsingContext& ctx) noexcept
    {
        if (ctx.scopes.top().type != ScopeType::conditional) {
            Logger::error("Mismatched header/footer type: should have 'conditional' type!\n");
            return ParserErrorCode::mismatched_header_footer_type;
        }

        ctx.conditionals.pop();
        ctx.scopes.pop();
        return ParserErrorCode::ok;
    }

    [[nodiscard]] static ParseExpressionResult
    handle_relational_operator(LineView lhs_tokens, LineView rhs_tokens, RelationalOperatorData::Operation op) noexcept
    {
        TRY_GET_SUBEXPRESSION(lhs);
        TRY_GET_SUBEXPRESSION(rhs);

        if (lhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Left-hand side of relational operator does not have 'number' type, has '",
                lhs_node.value_type(),
                "' instead!\n");
            return ParserErrorCode::relational_op_lhs_not_number_type;
        }

        if (rhs_node.value_type() != ValueType::number) {
            Logger::error(
                "Right-hand side of relational operator does not have 'number' type, has '",
                rhs_node.value_type(),
                "' instead!\n");
            return ParserErrorCode::relational_op_rhs_not_number_type;
        }

        return AST_Node{RelationalOperatorData{{}, lhs_node, rhs_node, op}};
    }

    [[nodiscard]] static ParseExpressionResult handle_loop_header(LineView condition_tokens, ParsingContext& ctx) noexcept
    {
        TRY_GET_SUBEXPRESSION(condition);

        if (condition_node.value_type() != ValueType::boolean) {
            Logger::error(
                "Condition of while loop does not have 'boolean' type, has type '", condition_node.value_type(), "' instead!\n");
            return ParserErrorCode::loop_condition_not_boolean_type;
        }

        AST_Node node{LoopData{{}, std::move(condition_node)}};

        ctx.scopes.push(Scope{ScopeType::loop, node.to_ref<LoopData>().body});
        ctx.new_scope_started = true;

        return node;
    }

    [[nodiscard]] static ParseExpressionResult handle_loop_footer(ParsingContext& ctx) noexcept
    {
        if (ctx.scopes.top().type != ScopeType::loop) {
            Logger::error("Mismatched header/footer type: should have 'loop' type!\n");
            return ParserErrorCode::mismatched_header_footer_type;
        }
        ctx.scopes.pop();
        return ParserErrorCode::ok;
    }

    [[nodiscard]] static ParseExpressionResult
    handle_number(const Token& number_token, bool is_negative, ParsingContext& /**/) noexcept
    {
        const auto& value_str = *number_token.content;

        double value{0};
        //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): converting from std::string to C-style string is a pain
        if (const auto [_, ec] = Raychel::from_chars(value_str.c_str(), value_str.c_str() + value_str.size(), value);
            ec != std::errc{}) {
            Logger::error("Unable to interpret string '", value_str, "' as a number!\n");
            return ParserErrorCode::invalid_numeric_constant;
        }

        return AST_Node{NumericConstantData{{}, is_negative ? -value : value}};
    }

    static void push_node(AST_Node&& node, AST& ast, ParsingContext& ctx) noexcept
    {
        if (ctx.scopes.empty()) {
            ast.nodes.emplace_back(std::move(node));
            return;
        }

        //TODO: make this more efficent (if it starts causing slowdowns)
        //we copy the data just to move it back, that is really silly

        auto& parent_node = ctx.scopes.top();
        switch (parent_node.type()) {
            case NodeType::conditional_construct:
            {
                auto data = parent_node.to_node_data<ConditionalConstructData>();
                if (!ctx.is_in_else_block)
                    data.body.emplace_back(std::move(node));
                else
                    data.else_body.emplace_back(std::move(node));
                parent_node = AST_Node{std::move(data)};
                break;
            }
            case NodeType::loop:
            {
                auto data = parent_node.to_node_data<LoopData>();
                data.body.emplace_back(std::move(node));
                parent_node = AST_Node{std::move(data)};
                break;
            }
            default:
                RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    ParseResult parse_body_block(const SourceTokens& source_tokens, AST& ast) noexcept
    {
        ParsingContext ctx;
        ParserErrorCode ec{};

        ctx.scopes.push(Scope{ScopeType::global, ast.nodes});

        std::for_each(source_tokens.begin(), source_tokens.end(), [&](const LineTokens& tokens) {
            if (ec != ParserErrorCode::ok) {
                return;
            }
            auto top_node_or_error = parse_statement_or_expression(tokens, ctx);

            if (const auto* error_code = std::get_if<ParserErrorCode>(&top_node_or_error); error_code) {
                ec = *error_code;
                return;
            }

            if (ctx.new_scope_started) {
                auto top_scope = ctx.scopes.top();
                RAYCHEL_ASSERT(top_scope.type != ScopeType::global);
                ctx.scopes.pop();
                ctx.scopes.top().nodes.get().push_back(Raychel::get<AST_Node>(std::move(top_node_or_error)));
                ctx.scopes.push(top_scope);

                ctx.new_scope_started = false;
            } else {
                ctx.scopes.top().nodes.get().push_back(Raychel::get<AST_Node>(std::move(top_node_or_error)));
            }
        });

        if (ec != ParserErrorCode::ok) {
            return ec;
        }

        if (ctx.scopes.size() != 1U) {
            Logger::error("Mismatched header/footer constructs: too many headers!\n");
            return ParserErrorCode::mismatched_header_footer_type;
        }

        return ast;
    }

} // namespace RaychelScript::Parser
