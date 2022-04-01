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

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "Concepts.h"
#include "StateFlags.h"

namespace RaychelScript::Interpreter {

    template <Descriptor ConstantT, Descriptor VariableT>
    struct InterpreterState
    {

        static_assert(
            std::is_same_v<typename ConstantT::value_type, typename VariableT::value_type>,
            "Constant value type and descriptor value type need to be the same!");

        using RegisterType = typename ConstantT::value_type;
        using ConstantContainer = std::vector<ConstantT>;
        using VariableContainer = std::vector<VariableT>;
        using DescriptorTable = std::unordered_map<std::string, DescriptorID>;

        struct Registers
        {
            RegisterType a{}, b{};
            RegisterType result{};

            StateFlags flags{StateFlags::none};
        };

        struct Snapshot
        {
            std::size_t end_of_outer_scope_constants;
            std::size_t end_of_outer_scope_variables;
        };

        ConstantContainer constants;
        VariableContainer variables;
        Registers registers;

        DescriptorTable _descriptor_table;
        DescriptorID _current_descriptor;
        std::stack<Snapshot, std::vector<Snapshot>> _stack_snapshots;
        bool _load_references{false};
    };
} //namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_INTERPRETER_STATE_H
