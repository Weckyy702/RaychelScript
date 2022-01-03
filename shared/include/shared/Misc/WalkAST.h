/**
* \file WalkAST.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AST utility functions
* \date 2021-12-05
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
#ifndef RAYCHELSCRIPT_WALK_AST_H
#define RAYCHELSCRIPT_WALK_AST_H

#include "shared/AST/AST.h"
#include "shared/AST/NodeData.h"

#include <concepts>
#include <functional>

namespace RaychelScript {

    struct NodeHandlers
    {
        template <typename T>
        using Handler = std::function<void(const T&)>;

        //NOLINTBEGIN(clang-analyzer-cplusplus.Move): false positive?

        template <typename F>
        explicit NodeHandlers(F f)
            : handle_assignment{std::forward<F>(f)},
              handle_variable_decl{std::forward<F>(f)},
              handle_variable_ref{std::forward<F>(f)},
              handle_arithmetic_operator{std::forward<F>(f)},
              handle_update_expression{std::forward<F>(f)},
              handle_numeric_constant{std::forward<F>(f)},
              handle_unary_operator{std::forward<F>(f)},
              handle_conditional_construct{std::forward<F>(f)},
              handle_literal_true{std::forward<F>(f)},
              handle_literal_false{std::forward<F>(f)},
              handle_relational_operator{std::forward<F>(f)},
              handle_inline_state_push{std::forward<F>(f)},
              handle_inline_state_pop{std::forward<F>(f)},
              handle_loop{std::forward<F>(f)}
        {}

        //NOLINTEND(clang-analyzer-cplusplus.Move)

        Handler<AssignmentExpressionData> handle_assignment;
        Handler<VariableDeclarationData> handle_variable_decl;
        Handler<VariableReferenceData> handle_variable_ref;
        Handler<ArithmeticExpressionData> handle_arithmetic_operator;
        Handler<UpdateExpressionData> handle_update_expression;
        Handler<NumericConstantData> handle_numeric_constant;
        Handler<UnaryExpressionData> handle_unary_operator;
        Handler<ConditionalConstructData> handle_conditional_construct;
        Handler<LiteralTrueData> handle_literal_true;
        Handler<LiteralFalseData> handle_literal_false;
        Handler<RelationalOperatorData> handle_relational_operator;
        Handler<InlinePushData> handle_inline_state_push;
        Handler<InlinePopData> handle_inline_state_pop;
        Handler<LoopData> handle_loop;
    };

    namespace details {

        template <std::invocable<const AST_Node&> F>
        void handle_node(const AST_Node& node, F&& f) noexcept;

        template <std::invocable<const AST_Node&> F>
        void handle(const AssignmentExpressionData& data, F&& f) noexcept
        {
            handle_node(data.lhs, std::forward<F>(f));
            handle_node(data.rhs, std::forward<F>(f));
        }

        template <std::invocable<const AST_Node&> F>
        void handle(const ArithmeticExpressionData& data, F&& f) noexcept
        {
            handle_node(data.lhs, std::forward<F>(f));
            handle_node(data.rhs, std::forward<F>(f));
        }

        template <std::invocable<const AST_Node&> F>
        void handle(const UnaryExpressionData& data, F&& f) noexcept
        {
            handle_node(data.value_node, std::forward<F>(f));
        }

        template <std::invocable<const AST_Node&> F>
        void handle(const ConditionalConstructData& data, F&& f) noexcept
        {
            handle_node(data.condition_node, std::forward<F>(f));
            for (const auto& node : data.body) {
                handle_node(node, std::forward<F>(f));
            }
        }

        template <std::invocable<const AST_Node&> F>
        void handle(const RelationalOperatorData& data, F&& f) noexcept
        {
            handle_node(data.lhs, std::forward<F>(f));
            handle_node(data.rhs, std::forward<F>(f));
        }

        template <std::invocable<const AST_Node&> F>
        void handle_node(const AST_Node& node, F&& f) noexcept
        {
            f(node);

            switch (node.type()) {
                case NodeType::assignment:
                    handle(node.to_node_data<AssignmentExpressionData>(), std::forward<F>(f));
                    break;
                case NodeType::arithmetic_operator:
                    handle(node.to_node_data<ArithmeticExpressionData>(), std::forward<F>(f));
                    break;
                case NodeType::unary_operator:
                    handle(node.to_node_data<UnaryExpressionData>(), std::forward<F>(f));
                    break;
                case NodeType::conditional_construct:
                    handle(node.to_node_data<ConditionalConstructData>(), std::forward<F>(f));
                    break;
                case NodeType::relational_operator:
                    handle(node.to_node_data<RelationalOperatorData>(), std::forward<F>(f));
                    break;
                default:
                    return;
            }
        }
    } // namespace details

    template <std::invocable<const AST_Node&> F>
    void for_each_node(const AST& ast, F&& f) noexcept
    {
        for (const auto& node : ast.nodes) {
            details::handle_node(node, std::forward<F>(f));
        }
    }

    template <std::invocable<const AST_Node&> F>
    void for_each_top_node(const AST& ast, F&& f) noexcept
    {
        for (const auto& node : ast.nodes) {
            f(node);
        }
    }

    inline void apply_handler(const AST_Node& node, const NodeHandlers& handlers) noexcept
    {
        switch (node.type()) {
            case NodeType::assignment:
                return handlers.handle_assignment(node.to_node_data<AssignmentExpressionData>());
            case NodeType::variable_decl:
                return handlers.handle_variable_decl(node.to_node_data<VariableDeclarationData>());
            case NodeType::variable_ref:
                return handlers.handle_variable_ref(node.to_node_data<VariableReferenceData>());
            case NodeType::arithmetic_operator:
                return handlers.handle_arithmetic_operator(node.to_node_data<ArithmeticExpressionData>());
            case NodeType::update_expression:
                return handlers.handle_update_expression(node.to_node_data<UpdateExpressionData>());
            case NodeType::numeric_constant:
                return handlers.handle_numeric_constant(node.to_node_data<NumericConstantData>());
            case NodeType::unary_operator:
                return handlers.handle_unary_operator(node.to_node_data<UnaryExpressionData>());
            case NodeType::conditional_construct:
                return handlers.handle_conditional_construct(node.to_node_data<ConditionalConstructData>());
            case NodeType::literal_true:
                return handlers.handle_literal_true(node.to_node_data<LiteralTrueData>());
            case NodeType::literal_false:
                return handlers.handle_literal_false(node.to_node_data<LiteralFalseData>());
            case NodeType::relational_operator:
                return handlers.handle_relational_operator(node.to_node_data<RelationalOperatorData>());
            case NodeType::inline_state_push:
                return handlers.handle_inline_state_push(node.to_node_data<InlinePushData>());
            case NodeType::inline_state_pop:
                return handlers.handle_inline_state_pop(node.to_node_data<InlinePopData>());
            case NodeType::loop:
                return handlers.handle_loop(node.to_node_data<LoopData>());
        }
    }

    inline void for_each_top_node(const AST& ast, const NodeHandlers& handlers) noexcept
    {
        for (const auto& node : ast.nodes) {
            apply_handler(node, handlers);
        }
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_WALK_AST_H