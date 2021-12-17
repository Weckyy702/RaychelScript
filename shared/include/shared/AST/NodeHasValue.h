/**
* \file NodeHasValue.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for node_has_value function
* \date 2021-12-06
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
#ifndef RAYCHELSCRIPT_AST_NODE_HAS_VALUE_H
#define RAYCHELSCRIPT_AST_NODE_HAS_VALUE_H

#include "AST_Node.h"

namespace RaychelScript {

    [[nodiscard]] inline bool node_has_known_value(const AST_Node& node) noexcept
    {
        switch (node.type()) {
            case NodeType::literal_true:
            case NodeType::literal_false:
            case NodeType::numeric_constant:
                return true;
            default:
                return false;
        }
    }

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_AST_NODE_HAS_VALUE_H