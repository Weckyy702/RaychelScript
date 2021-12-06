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

#include "AST/AST.h"
#include "AST/NodeData.h"

#include <concepts>

namespace RaychelScript {

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
            handle_node(data.value, std::forward<F>(f));
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
                    break;
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

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_WALK_AST_H