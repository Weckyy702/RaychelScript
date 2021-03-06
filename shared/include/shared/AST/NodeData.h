/**
* \file NodeData.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for NodeData structs
* \date 2021-10-01
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
#ifndef RAYCHELSCRIPT_NODE_DATA_H
#define RAYCHELSCRIPT_NODE_DATA_H

#include <string>
#include <string_view>
#include <vector>
#include "AST_Node.h"
#include "NodeType.h"
#include "ValueType.h"

namespace RaychelScript {

    /**
    * \brief Base class for all types satisfying the NodeData concept
    *
    * \note Inheriting from this class is not required but encouraged
    *
    * \refitem NodeData
    *
    * \tparam _type Type of the node
    * \tparam _is_lvalue If the node is an lvalue (something that is assignable)
    */
    template <NodeType _type, ValueType _value_type, bool _is_lvalue = false, bool _has_side_effect = false>
    struct NodeDataBase
    {
        static constexpr auto type = _type;
        static constexpr auto value_type = _value_type;
        static constexpr auto is_lvalue = _is_lvalue;
        static constexpr auto has_side_effect = _has_side_effect;
    };

    struct AssignmentExpressionData : NodeDataBase<NodeType::assignment, ValueType::none, false, true>
    {
        AST_Node lhs;
        AST_Node rhs;
    };

    struct ArithmeticExpressionData : NodeDataBase<NodeType::arithmetic_operator, ValueType::number>
    {
        enum class Operation {
            add = 1,
            subtract,
            multiply,
            divide,
            power,
        };
        AST_Node lhs;
        AST_Node rhs;
        Operation operation{};
    };

    struct UpdateExpressionData : NodeDataBase<NodeType::update_expression, ValueType::none, false, true>
    {
        using Operation = ArithmeticExpressionData::Operation;

        AST_Node lhs;
        AST_Node rhs;
        Operation operation{};
    };

    struct VariableDeclarationData : NodeDataBase<NodeType::variable_decl, ValueType::variable_ref, true, true>
    {
        std::string name{};
        bool is_const{};
    };

    struct VariableReferenceData : NodeDataBase<NodeType::variable_ref, ValueType::number, true>
    {
        std::string name{};
    };

    struct NumericConstantData : NodeDataBase<NodeType::numeric_constant, ValueType::number, false>
    {
        long double value{};
    };

    struct UnaryExpressionData : NodeDataBase<NodeType::unary_operator, ValueType::number>
    {
        enum class Operation {
            minus = 1,
            plus,
            factorial,
            magnitude,
        };

        AST_Node value_node;
        Operation operation{};
    };

    struct ConditionalConstructData : NodeDataBase<NodeType::conditional_construct, ValueType::none, false, true>
    {
        AST_Node condition_node;
        std::vector<AST_Node> body{};
        std::vector<AST_Node> else_body{};
    };

    struct RelationalOperatorData : NodeDataBase<NodeType::relational_operator, ValueType::boolean>
    {
        enum class Operation { equals = 1, not_equals, less_than, greater_than };

        AST_Node lhs;
        AST_Node rhs;
        Operation operation{};
    };

    struct InlinePushData : NodeDataBase<NodeType::inline_state_push, ValueType::none, false, true>
    {};

    struct InlinePopData : NodeDataBase<NodeType::inline_state_pop, ValueType::none, false, true>
    {};

    struct LoopData : NodeDataBase<NodeType::loop, ValueType::none, false, true>
    {
        AST_Node condition_node;
        std::vector<AST_Node> body{};
    };

    struct FunctionCallData : NodeDataBase<NodeType::function_call, ValueType::number, false, true>
    {
        std::string mangled_callee_name;
        std::vector<AST_Node> argument_expressions;
    };

    struct FunctionReturnData : NodeDataBase<NodeType::function_return, ValueType::none, false, true>
    {
        AST_Node return_value;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_NODE_DATA_H
