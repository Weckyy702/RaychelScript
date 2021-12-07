/**
* \file VariableDescriptor.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VariableDescriptor class
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
#ifndef RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H
#define RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H

#include <concepts>

#include "DescriptorID.h"

namespace RaychelScript {

    template <std::floating_point T>
    class VariableDescriptor
    {
    public:

        using value_type = std::remove_cvref_t<T>;

        VariableDescriptor() = default;

        explicit VariableDescriptor(T value) : value_{value}
        {}

        [[nodiscard]] auto id() const noexcept
        {
            return id_;
        }

        [[nodiscard]] T& value() noexcept
        {
            return value_;
        }

        [[nodiscard]] const T& value() const noexcept
        {
            return value_;
        }

    private:
        DescriptorID id_{this};
        T value_{};
    };

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H