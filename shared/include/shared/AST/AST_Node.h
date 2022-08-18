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
#include <optional>
#include <type_traits>

#include "RaychelCore/ClassMacros.h"
#include "RaychelCore/Raychel_assert.h"

#include "NodeType.h"
#include "ValueType.h"

namespace RaychelScript {

    /**
    * \brief Specifies that a type has all members needed to use it inside an AST_Node object
    */
    template <typename T>
    concept NodeData = requires()
    {
        std::is_standard_layout_v<T>&& std::is_trivial_v<T>; //equivalent to the deprecated std::is_pod_v

        T::type;
        std::is_same_v<std::remove_cvref_t<decltype(T::type)>, NodeType>;

        T::value_type;
        std::is_same_v<std::remove_cvref_t<decltype(T::value_type)>, ValueType>;

        T::is_lvalue;
        std::is_same_v<std::remove_cvref_t<decltype(T::is_lvalue)>, bool>;

        T::has_side_effect;
        std::is_same_v<std::remove_cvref_t<decltype(T::has_side_effect)>, bool>;
    };

    /**
    * brief Class for representing parsed AST nodes
    *
    * This class represents a single Node in the AST. All the actual AST structure is handled by the NodeData held inside this object
    * An AST_Node can hold a value of any type that satisfies NodeData. It also holds the type of the data it contains
    *
    */
    class AST_Node
    {
    public:
        template <NodeData T>
        explicit AST_Node(T&& data) //NOLINT(bugprone-forwarding-reference-overload): Our template parameter is constrained
            : type_{T::type},
              data_{std::forward<T>(data)},
              value_type_{T::value_type},
              is_lvalue_{T::is_lvalue},
              has_side_effect_{T::has_side_effect}
        {}

        [[nodiscard]] NodeType type() const noexcept
        {
            return type_;
        }

        template <NodeData T>
        [[nodiscard]] T to_node_data() const noexcept
        {
            RAYCHEL_ASSERT(type_ == T::type);
            return std::any_cast<T>(data_);
        }

        template <NodeData T>
        [[nodiscard]] T& to_ref() noexcept
        {
            RAYCHEL_ASSERT(type_ == T::type);
            return std::any_cast<T&>(data_);
        }

        [[nodiscard]] ValueType value_type() const noexcept
        {
            return value_type_;
        }

        [[nodiscard]] bool is_lvalue() const noexcept
        {
            return is_lvalue_;
        }

        [[nodiscard]] bool has_side_effect() const noexcept
        {
            return has_side_effect_;
        }

    private:
        NodeType type_;
        std::any data_;

        ValueType value_type_;
        bool is_lvalue_ : 1;
        bool has_side_effect_ : 1;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_AST_NODE_H
