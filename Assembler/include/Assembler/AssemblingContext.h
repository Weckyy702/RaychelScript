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

#include "shared/rasm/Instruction.h"

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace RaychelScript::Assembler {

    class AssemblingContext
    {

        using Immediates = std::vector<std::pair<double, Assembly::MemoryIndex>>;

        struct ScopeData
        {
            std::vector<std::string> names;
        };

    public:
        explicit AssemblingContext(std::vector<Assembly::Instruction>& instructions, Immediates& immediate_values)
            : instructions_{instructions}, immediate_values_{immediate_values}
        {}

        /**
        * \brief Return if the name has a memory index assigned to it
        * 
        * \param name name of a variable
        * \return true The name has a memory index associated with it
        * \return false The name does not have a memory index associated with it
        */
        [[nodiscard]] bool has_name(const std::string& name) const noexcept
        {
            return names_.find(name) != names_.end();
        }

        /**
        * \brief Return if the context has at least one scope on the stack
        * 
        * \return true The context has at least one scope on the stack
        * \return false The context has no scopes on the stack
        */
        [[nodiscard]] bool has_scopes() const noexcept
        {
            return !scopes_.empty();
        }

        /**
        * \brief Return if the index points to an intermediate location
        * 
        * \param index Index of a memory location
        * \return true The index points to an intermediate value
        * \return false The index does not point to an intermediate value
        */
        [[nodiscard]] bool is_intermediate(std::uint8_t index) const noexcept
        {
            return std::find_if(intermediates_.begin(), intermediates_.end(), [&](const auto& intermediate) {
                       return intermediate.second.value() == index;
                   }) != intermediates_.end();
        }

        /**
        * \brief Return the memory index associated with a name
        * 
        * \param name Name of a variable
        * \return Assembly::MemoryIndex memory index for the name
        */
        [[nodiscard]] Assembly::MemoryIndex memory_index_for(const std::string& name) const noexcept
        {
            RAYCHEL_ASSERT(has_name(name));
            return names_.find(name)->second;
        }

        /**
        * \brief Return the index for the reserved A register
        */
        [[nodiscard]] static constexpr Assembly::MemoryIndex result_index() noexcept
        {
            return Assembly::make_memory_index(0);
        }

        /**
        * \brief Return the index to the last instruction
        */
        [[nodiscard]] auto instruction_index() const noexcept
        {
            return instructions_.size() - 1;
        }

        /**
        * \brief Return a reference to the internal instruction buffer
        */
        [[nodiscard]] auto& instructions() noexcept
        {
            return instructions_;
        }

        /**
        * \brief Return a constant reference to the internal instruction buffer
        */
        [[nodiscard]] const auto& instructions() const noexcept
        {
            return instructions_;
        }

        /**
        * \brief Add a new instruction to the buffer and return its index
        * 
        * \tparam code op code of the instruction
        * \tparam Ts Constructor arguments for the instruction. Must match the number expected for the op code
        */
        template <Assembly::OpCode code, typename... Ts>
        auto emit(const Ts&... args) noexcept
        {
            static_assert(
                sizeof...(Ts) == Assembly::number_of_arguments(code),
                "Number of instructions arguments does not match number required!");
            instructions_.emplace_back(Assembly::Instruction{code, args...});
            return instruction_index();
        }

        void push_scope() noexcept
        {
            scopes_.emplace();
        }

        void pop_scope() noexcept
        {
            RAYCHEL_ASSERT(!scopes_.empty());
            const auto& scope = scopes_.top();

            for (const auto& name : scope.names) {
                names_.erase(name);
            }

            scopes_.pop();
        }

        /**
        * \brief Allocate a new variable and return its memory index
        * 
        * \param name Name of the variable
        */
        [[nodiscard]] auto allocate_variable(const std::string& name) noexcept
        {
            const auto [did_insert, index] = _allocate_new(names_, name);
            if (did_insert && !scopes_.empty()) {
                scopes_.top().names.emplace_back(name);
            }
            return index;
        }

        /**
        * \brief Allocate a new immediate value and return its memory index
        * 
        * \param value immediate value
        */
        [[nodiscard]] auto allocate_immediate(double value) noexcept
        {
            const auto [did_insert, index] = _allocate_new(immediates_, value);
            if (did_insert) {
                immediate_values_.emplace_back(std::make_pair(value, index));
            }
            return index;
        }

        /**
        * \brief allocate a new intermediate value and return its memory index
        */
        [[nodiscard]] auto allocate_intermediate() noexcept
        {
            for (auto& [free, index] : intermediates_) {
                if (free) {
                    free = false;
                    return index;
                }
            }

            auto idx = _new_index();
            intermediates_.emplace_back(false, idx);
            return idx;
        }

        /**
        * \brief Mark all intermediate values as free so they can be reused
        */
        void free_intermediates() noexcept
        {
            for (auto& [free, _] : intermediates_) {
                free = true;
            }
        }

        [[nodiscard]] auto number_of_memory_locations() const noexcept
        {
            RAYCHEL_ASSERT(std::cmp_less_equal(current_index_, std::numeric_limits<std::uint8_t>::max()));
            return static_cast<std::uint8_t>(current_index_);
        }

    private:
        /**
        * \brief Return the memory index for the given value. If none is assigned yet, allocate a new one
        * 
        * \tparam Container Type of the container
        * \tparam T value type of the container
        * \param container container to search in
        * \param value value to search for
        */
        template <typename Container, typename T = typename Container::value_type>
        [[nodiscard]] std::pair<bool, Assembly::MemoryIndex> _allocate_new(Container& container, const T& value) noexcept
        {
            if (const auto it = container.find(value); it != container.end()) {
                return {false, it->second};
            }
            const auto it = container.insert({value, _new_index()});
            return {true, it.first->second};
        }

        [[nodiscard]] Assembly::MemoryIndex _new_index() noexcept
        {
            return Assembly::make_memory_index(current_index_++);
        }

        std::vector<Assembly::Instruction>& instructions_;
        Immediates& immediate_values_;
        std::vector<std::pair<bool, Assembly::MemoryIndex>> intermediates_;

        std::size_t current_index_{1}; //the first index is reserved for the A register

        std::unordered_map<std::string, Assembly::MemoryIndex> names_;
        std::unordered_map<double, Assembly::MemoryIndex> immediates_;

        std::stack<ScopeData, std::vector<ScopeData>> scopes_;
    };

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H
