/**
* \file ExecutionState.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ExecutionState class
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
#ifndef RAYCHELSCRIPT_EXECUTION_STATE_H
#define RAYCHELSCRIPT_EXECUTION_STATE_H

#include <map>
#include <string>
#include <vector>

#include "ConstantDescriptor.h"
#include "VariableDescriptor.h"

#define RAYCHELSCRIPT_BIT(x) (1U << (x))

namespace RaychelScript {

    enum class StateFlags : std::uint32_t {
        none = 0,
        zero = RAYCHELSCRIPT_BIT(1U),
        negative = RAYCHELSCRIPT_BIT(2U),
    };

    inline StateFlags& operator|=(StateFlags& lhs, StateFlags rhs) noexcept
    {
        using T = std::underlying_type_t<StateFlags>;

        lhs = StateFlags{static_cast<T>(lhs) | static_cast<T>(rhs)};
        return lhs;
    }

    inline StateFlags operator&(StateFlags lhs, StateFlags rhs) noexcept
    {
        using T = std::underlying_type_t<StateFlags>;
        return StateFlags{static_cast<T>(lhs) & static_cast<T>(rhs)};
    }

    template <std::floating_point T>
    struct StateRegisters
    {
        T a{}, b{};
        T result{};

        StateFlags flags{StateFlags::none};
    };

    template <std::floating_point T>
    struct ExecutionState
    {
        std::vector<ConstantDescriptor<T>> constants;
        std::vector<VariableDescriptor<T>> variables;

        std::map<std::string, DescriptorID> _descriptor_table;
        StateRegisters<T> _registers;
        DescriptorID _current_descriptor;
        bool _load_references{false};
    };

    [[nodiscard]] std::string
    get_descriptor_identifier(const std::map<std::string, DescriptorID>& descriptor_table, const DescriptorID& id) noexcept;

    template <typename T>
    [[nodiscard]] std::string get_descriptor_identifier(const ExecutionState<T>& state, const DescriptorID& id) noexcept
    {
        return get_descriptor_identifier(state._descriptor_table, id);
    }
} //namespace RaychelScript

#undef RAYCHELSCRIPT_BIT

#endif //!RAYCHELSCRIPT_EXECUTION_STATE_H