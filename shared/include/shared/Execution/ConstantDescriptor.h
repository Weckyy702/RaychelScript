/**
* \file ConstantDescriptor.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ConstantDescriptor class
* \date 2021-11-16
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
#ifndef RAYCHELSCRIPT_CONSTANT_DESCRIPTOR_H
#define RAYCHELSCRIPT_CONSTANT_DESCRIPTOR_H

#include <concepts>

#include "DescriptorID.h"

namespace RaychelScript {

    template <std::floating_point T>
    class ConstantDescriptor
    {

    public:
        using value_type = std::remove_cvref_t<T>;

        ConstantDescriptor() = default;

        explicit ConstantDescriptor(T value) : has_value_set_{true}, value_{value}
        {}

        [[nodiscard]] auto id() const noexcept
        {
            return id_;
        }

        [[nodiscard]] T value() const noexcept
        {
            return value_;
        }

        [[nodiscard]] bool has_value_set() const noexcept
        {
            return has_value_set_;
        }

        void set_value(T value) noexcept
        {
            if (!has_value_set_) {
                value_ = value;
                has_value_set_ = true;
            }
        }

    private:
        DescriptorID id_{this};

        bool has_value_set_{false};
        T value_{};
    };

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_CONSTANT_DESCRIPTOR_H
