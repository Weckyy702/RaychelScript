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
    * \tparam _is_value_ref If the node is a value reference (something that holds a value and is assignable)
    */
    template <NodeType _type, ValueType _value_type, bool _is_value_ref = false, bool _is_constant = false, bool _has_known_value = false>
    struct NodeDataBase
    {
        static constexpr auto type = _type;
        static constexpr auto value_type = _value_type;
        static constexpr auto is_value_ref = _is_value_ref;
    };

    struct AssignmentExpressionData : NodeDataBase<NodeType::assignment, ValueType::none>
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

    struct VariableDeclarationData : NodeDataBase<NodeType::variable_decl, ValueType::none, true>
    {
        std::string name;
        bool is_const;
    };

    struct VariableReferenceData : NodeDataBase<NodeType::variable_ref, ValueType::number, true>
    {
        std::string name;
    };

    struct NumericConstantData : NodeDataBase<NodeType::numeric_constant, ValueType::number, true>
    {
        long double value;
    };

    struct UnaryExpressionData : NodeDataBase<NodeType::unary_operator, ValueType::number>
    {
        enum class Operation {
            minus=1,
            plus,
            factorial,
            magnitude,
        };

        AST_Node value;
        Operation operation{};
    };

    struct ConditionalConstructData : NodeDataBase<NodeType::conditional_construct, ValueType::none>
    {
        AST_Node condition_node;
        std::vector<AST_Node> body{};

        //TODO: else blocks
    };

    struct LiteralTrueData : NodeDataBase<NodeType::literal_true, ValueType::boolean, true>
    {};

    struct LiteralFalseData : NodeDataBase<NodeType::literal_false, ValueType::boolean, true>
    {};

    struct RelationalOperatorData : NodeDataBase<NodeType::relational_operator, ValueType::boolean>
    {
        enum class Operation {
            equals=1,
            not_equals,
            less_than,
            greater_than
        };

        AST_Node lhs;
        AST_Node rhs;
        Operation operation{};
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_NODE_DATA_H