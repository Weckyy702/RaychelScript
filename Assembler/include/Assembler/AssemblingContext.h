/**
* \file AssemblingContext.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssemblingContext class
* \date 2021-12-21
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
#ifndef RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H
#define RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H

#include "AssemblerErrorCode.h"
#include "shared/AST/AST.h"
#include "shared/AST/FunctionData.h"
#include "shared/Misc/Scope.h"
#include "shared/VM/VMData.h"
#include "shared/rasm/Instruction.h"

#include "RaychelCore/Finally.h"

#include <queue>
#include <set>
#include <string>
#include <variant>

namespace RaychelScript::Assembler {

    using namespace Assembly; //NOLINT(google-build-using-namespace)

    template <typename T>
    using ErrorOr = std::variant<AssemblerErrorCode, T>;

    class AssemblingContext
    {

        struct MarkedFunction
        {
            /*implicit*/ MarkedFunction(std::string _mangled_name, std::ptrdiff_t _index = 0) //NOLINT
                : mangled_name{std::move(_mangled_name)}, index{_index}
            {}

            std::string mangled_name{};
            std::ptrdiff_t index{};

            constexpr auto operator<=>(const MarkedFunction& other) const noexcept
            {
                return mangled_name <=> other.mangled_name;
            }
        };

    public:
        using Scope = BasicScope<MemoryIndex, std::string, std::queue<MemoryIndex>>;

        explicit AssemblingContext(const AST& _ast, VM::VMData& data) : ast{_ast}, data_{data}
        {
            //push the global scope
            push_function_scope("__global");
        }

        [[nodiscard]] constexpr MemoryIndex a_index() //NOLINT(readability-convert-member-functions-to-static)
        {
            return make_memory_index(0U, MemoryIndex::ValueType::stack);
        }

        ErrorOr<MemoryIndex> add_variable(std::string name)
        {
            if (has_identifier(scopes_, name))
                return AssemblerErrorCode::duplicate_name;
            return _current_scope().lookup_table.emplace(std::move(name), _new_stack_index()).first->second;
        }

        MemoryIndex allocate_intermediate()
        {
            if (_current_scope().scope_data.empty()) {
                return _new_intermediate_index();
            }
            auto tmp = _current_scope().scope_data.front();
            _current_scope().scope_data.pop();
            return tmp;
        }

        void free_intermediate(MemoryIndex index)
        {
            if (index.type() != MemoryIndex::ValueType::intermediate)
                return;
            _current_scope().scope_data.push(index);
        }

        MemoryIndex allocate_immediate(double x)
        {
            for (std::size_t i{}; i != data_.immediate_values.size(); ++i) {
                if (data_.immediate_values.at(i) == x) {
                    return make_memory_index(i, MemoryIndex::ValueType::immediate);
                }
            }
            data_.immediate_values.emplace_back(x);
            return make_memory_index(data_.immediate_values.size() - 1, MemoryIndex::ValueType::immediate);
        }

        ErrorOr<MemoryIndex> index_for(const std::string& name)
        {
            auto maybe_index = find_identifier(scopes_, name);
            if (!maybe_index.has_value())
                return AssemblerErrorCode::unresolved_identifier;
            return maybe_index.value();
        }

        void push_scope(bool inherits_from_parent_scope, std::string_view name)
        {
            Logger::debug("Pushing new scope with name '", name, "'\n");
            scopes_.emplace_back(
                Scope{.inherits_from_parent_scope = inherits_from_parent_scope, .scope_data = {}, .lookup_table = {}});
        }

        AssemblerErrorCode pop_scope(std::string_view name)
        {
            if (scopes_.size() == 1)
                return AssemblerErrorCode::invalid_scope_pop;
            scopes_.pop_back();

            Logger::debug("Popping scope with name '", name, "'\n");

            return AssemblerErrorCode::ok;
        }

        void push_function_scope(std::string_view name)
        {
            current_frame_ = &data_.call_frames.emplace_back();
            push_scope(false, name);
        }

        void pop_function_scope(std::string_view name)
        {
            current_frame_ = &data_.call_frames.front();
            (void)pop_scope(name);
        }

        template <OpCode op_code>
        auto emit(MemoryIndex a = {}, MemoryIndex b = {})
        {
            current_frame_->instructions.emplace_back(op_code, a, b);
            return current_frame_->instructions.size() - 1U;
        }

        [[nodiscard]] std::string_view indent() const
        {
            return std::string_view{
                "..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|."
                ".|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|.."
                "|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|..|",
                debug_depth * 3};
        }

        [[nodiscard]] auto& instructions()
        {
            return current_frame_->instructions;
        }

        [[nodiscard]] auto next_instruction_index()
        {
            return instructions().size();
        }

        ErrorOr<MemoryIndex> find_function(const std::string& mangled_name)
        {
            if (const auto it = all_marked_functions_.find(mangled_name); it != all_marked_functions_.end())
                return make_memory_index(it->index, MemoryIndex::ValueType::immediate);
            return AssemblerErrorCode::unresolved_identifier;
        }

        AssemblerErrorCode mark_function(const std::string& mangled_name)
        {
            Logger::debug("Marking function '", mangled_name, "'\n");
            if (all_marked_functions_.contains(mangled_name)) {
                return AssemblerErrorCode::ok;
            }
            if (const auto it = ast.functions.find(mangled_name); it != ast.functions.end()) {
                const auto [i, _] = all_marked_functions_.emplace(mangled_name, all_marked_functions_.size() + 1);
                Logger::debug("New function will get index ", i->index, '\n');
                marked_functions_.emplace(it->second);
                return AssemblerErrorCode::ok;
            }
            Logger::error("Tried to mark nonexistent function '", mangled_name, "'\n");
            return AssemblerErrorCode::unresolved_identifier;
        }

        bool has_marked_functions()
        {
            return !marked_functions_.empty();
        }

        auto next_marked_function()
        {
            auto next_function = std::move(marked_functions_.front());
            marked_functions_.pop();
            return next_function;
        }

        //NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
        const AST& ast;
        std::size_t debug_depth{};
        //NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)

    private:
        MemoryIndex _new_stack_index()
        {
            return _new_index(MemoryIndex::ValueType::stack);
        }

        MemoryIndex _new_intermediate_index()
        {
            return _new_index(MemoryIndex::ValueType::intermediate);
        }

        MemoryIndex _new_index(MemoryIndex::ValueType type)
        {
            return make_memory_index(current_frame_->size++, type);
        }

        [[nodiscard]] Scope& _current_scope()
        {
            return scopes_.back();
        }

        std::queue<FunctionData> marked_functions_{};
        std::set<MarkedFunction> all_marked_functions_{};
        VM::VMData& data_;
        VM::CallFrameDescriptor* current_frame_{};
        std::vector<Scope> scopes_{};
    };

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H
