/**
* \file Interpreter.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Interpreter functions
* \date 2021-11-15
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
#ifndef RAYCHELSCRIPT_INTERPRETER_H
#define RAYCHELSCRIPT_INTERPRETER_H

#ifdef _WIN32
    #ifdef RaychelScriptInterpreter_EXPORTS
        #define RAYCHELSCRIPT_INTERPRETER_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_INTERPRETER_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_INTERPRETER_API
#endif

#include "InterpreterErrorCode.h"
#include "InterpreterState.h"

#include <concepts>
#include <map>
#include <optional>
#include <string>
#include <variant>

#include "shared/AST/AST.h"
#include "shared/Execution/ConstantDescriptor.h"
#include "shared/Execution/VariableDescriptor.h"
#include "Parser/Parser.h"

#include "RaychelCore/AssertingGet.h"

#define RAYCHELSCRIPT_INTERPRETER_DEPRECATED RAYCHELSCRIPT_PARSER_DEPRECATED

namespace RaychelScript {

    template <Interpreter::Descriptor CD, Interpreter::Descriptor VD>
    [[nodiscard]] std::string
    get_descriptor_identifier(const Interpreter::InterpreterState<CD, VD>& state, const DescriptorID& id) noexcept
    {
        if (const auto it = std::find_if(
                state._descriptor_table.begin(),
                state._descriptor_table.end(),
                [&](const auto& pair) { return pair.second == id; });
            it != state._descriptor_table.end()) {
            return it->first;
        }
        return "<Descriptor not found>";
    }

    template <Interpreter::Descriptor CD, Interpreter::Descriptor VD>
    [[nodiscard]] auto get_identifier_value(const Interpreter::InterpreterState<CD, VD>& state, std::string_view name) noexcept
        -> Raychel::AssertingOptional<typename Interpreter::InterpreterState<CD, VD>::RegisterType>
    {
        if (const auto it = state._descriptor_table.find(std::string{name}); it != state._descriptor_table.end()) {
            const auto [_, descriptor] = *it;

            if (descriptor.is_constant()) {
                return state.constants.at(descriptor.index()).value();
            }
            return state.variables.at(descriptor.index()).value();
        }
        return {};
    }

    namespace Interpreter {

        template <std::floating_point T>
        using ExecutionResult =
            std::variant<InterpreterErrorCode, InterpreterState<ConstantDescriptor<T>, VariableDescriptor<T>>>;

        template <std::floating_point T>
        using ParameterMap = std::map<std::string, T>;

        template <std::floating_point T>
        RAYCHELSCRIPT_INTERPRETER_API [[nodiscard]] ExecutionResult<T>
        interpret(const AST& ast, const ParameterMap<T>& parameters) noexcept;

        namespace details {
            inline void _instantiate_entry_points_header() noexcept
            {
                [[maybe_unused]] auto _float = interpret<float>(AST{}, {});
                [[maybe_unused]] auto _double = interpret<double>(AST{}, {});
                [[maybe_unused]] auto _long_double = interpret<long double>(AST{}, {});
            }
        }
    } // namespace Interpreter

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_INTERPRETER_H