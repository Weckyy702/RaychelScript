/**
* \file AST_Node.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header class for AST_Node interface and child classes
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
#ifndef RAYCHELSCRIPT_AST_NODE_H
#define RAYCHELSCRIPT_AST_NODE_H

#include <any>
#include <concepts>
#include <memory>
#include <type_traits>

#include "RaychelCore/Raychel_assert.h"

#include "NodeType.h"

namespace RaychelScript {

    // clang-format off
    template <typename T>
    concept NodeData = requires()
    {
        std::is_standard_layout_v<T> && std::is_trivial_v<T>; //equivalent to the deprecated std::is_pod_v<>

        {
            T::type
        };
        std::is_same_v<std::remove_cvref_t<decltype(T::type)>, NodeType>;
        {
            T::is_value_ref
        };
        std::is_same_v<std::remove_cvref_t<decltype(T::is_value_ref)>, bool>;
        {
            T::has_known_value
        };
        std::is_same_v<std::remove_cvref_t<decltype(T::has_known_value)>, bool>;
    };
    // clang-format on

    class AST_Node
    {
    public:
        template <NodeData T>
        explicit AST_Node(const T& data) : type_{T::type}, data_{data}
        {}

        [[nodiscard]] NodeType type() const noexcept
        {
            return type_;
        }

        template <NodeData T>
        [[nodiscard]] T to_node_data() const noexcept
        {
            RAYCHEL_ASSERT(type() == T::type);
            return std::any_cast<T>(data_);
        }

    private:
        NodeType type_;
        std::any data_;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_AST_NODE_H