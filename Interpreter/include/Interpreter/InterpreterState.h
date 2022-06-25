/**
* \file State.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for State class
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

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "StateFlags.h"

namespace RaychelScript {
    struct AST;
} //namespace RaychelScript

namespace RaychelScript::Interpreter {

    namespace details {
        struct ValueData
        {
            std::size_t index_in_scope{};
            std::size_t index_in_scope_chain{};
            bool is_constant;
        };
    } // namespace details

    struct State
    {
        using DescriptorTable = std::unordered_map<std::string, details::ValueData>;

        struct Registers
        {
            double a{}, b{};
            double result{};

            StateFlags flags{StateFlags::none};
        };

        struct Scope
        {
            bool inherits_from_parent_scope;

            std::vector<std::optional<double>> constants{};
            std::vector<double> variables{};
            DescriptorTable descriptor_table{};
        };

        const AST& ast;

        Registers registers{};
        std::vector<Scope> scopes{};

        std::optional<details::ValueData> _current_descriptor{};
        bool _load_references{false};

        std::size_t indent{};
    };
} //namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_INTERPRETER_STATE_H
