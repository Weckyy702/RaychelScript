/**
* \file NodeType.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for NodeType enum
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
#ifndef RAYCHELSCRIPT_NODE_TYPE_H
#define RAYCHELSCRIPT_NODE_TYPE_H

namespace RaychelScript {
    enum class NodeType {
        assignment,
        variable_decl,
        variable_ref,
        arithmetic_operator,
        update_expression,
        numeric_constant,
        unary_operator,
        conditional_construct,
        relational_operator,
        inline_state_push,
        inline_state_pop,
        loop,
        function_call,
        function_return,
    };
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_NODE_TYPE_H
