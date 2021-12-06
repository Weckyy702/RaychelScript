/**
* \file InterpreterState.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for InterpreterState class
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
#ifndef RAYCHELSCRIPT_INTERPRETER_STATE_H
#define RAYCHELSCRIPT_INTERPRETER_STATE_H

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "Execution/ConstantDescriptor.h"
#include "Execution/VariableDescriptor.h"

#define RAYCHELSCRIPT_BIT(x) (1U << (x))

namespace RaychelScript::Interpreter {

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

    inline bool operator&(StateFlags lhs, StateFlags rhs) noexcept
    {
        using T = std::underlying_type_t<StateFlags>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }

    inline bool operator!(StateFlags flags) noexcept
    {
        return flags == StateFlags::none;
    }

    template <std::floating_point T>
    struct StateRegisters
    {
        T a{}, b{};
        T result{};

        StateFlags flags{StateFlags::none};
    };

    template <std::floating_point T>
    struct InterpreterState
    {
        using ConstantContainer = std::vector<ConstantDescriptor<T>>;
        using VariableContainer = std::vector<VariableDescriptor<T>>;
        using DescriptorTable = std::map<std::string, DescriptorID>;

        struct Snapshot
        {
            std::size_t end_of_outer_scope_constants;
            std::size_t end_of_outer_scope_variables;
        };

        ConstantContainer constants;
        VariableContainer variables;

        DescriptorTable _descriptor_table;
        StateRegisters<T> _registers;
        DescriptorID _current_descriptor;
        std::stack<Snapshot, std::vector<Snapshot>> _stack_snapshots;
        bool _load_references{false};
    };
} //namespace RaychelScript::Interpreter

#undef RAYCHELSCRIPT_BIT

#endif //!RAYCHELSCRIPT_INTERPRETER_STATE_H