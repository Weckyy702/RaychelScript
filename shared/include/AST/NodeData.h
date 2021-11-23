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
    template <NodeType _type, bool _is_value_ref = false, bool _is_constant = false, bool _has_known_value = false>
    struct NodeDataBase
    {
        static constexpr auto type = _type;
        static constexpr auto is_value_ref = _is_value_ref;
    };

    struct AssignmentExpressionData : NodeDataBase<NodeType::assignment>
    {
        AST_Node lhs;
        AST_Node rhs;
    };

    struct ArithmeticExpressionData : NodeDataBase<NodeType::math_op>
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

    struct VariableDeclarationData : NodeDataBase<NodeType::variable_decl, true>
    {
        std::string name;
        bool is_const;
    };

    struct VariableReferenceData : NodeDataBase<NodeType::variable_ref, true>
    {
        std::string name;
    };

    struct NumericConstantData : NodeDataBase<NodeType::numeric_constant, true>
    {
        double value;
    };
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_NODE_DATA_H