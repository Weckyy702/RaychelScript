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

#include "rasm/Instruction.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace RaychelScript::Assembler {

    class AssemblingContext
    {

        using Immediates = std::vector<std::pair<double, Assembly::MemoryIndex>>;

    public:
        explicit AssemblingContext(std::vector<Assembly::Instruction>& instructions, Immediates& immediate_values)
            : instructions_{instructions}, immediate_values_{immediate_values}
        {}

        [[nodiscard]] bool has_name(const std::string& name) const noexcept
        {
            return names_.find(name) != names_.end();
        }

        [[nodiscard]] Assembly::MemoryIndex memory_index_for(const std::string& name) const noexcept
        {
            RAYCHEL_ASSERT(has_name(name));
            return Assembly::make_memory_index(names_.find(name)->second);
        }

        [[nodiscard]] static constexpr Assembly::MemoryIndex a_index() noexcept
        {
            return Assembly::make_memory_index(0);
        }

        [[nodiscard]] static constexpr Assembly::MemoryIndex b_index() noexcept
        {
            return Assembly::make_memory_index(1);
        }

        [[nodiscard]] const auto& names() const noexcept
        {
            return names_;
        }

        [[nodiscard]] const auto& immediates() const noexcept
        {
            return immediates_;
        }

        [[nodiscard]] auto instruction_index() const noexcept
        {
            return instructions_.size() - 1;
        }

        template <Assembly::OpCode code, typename... Ts>
        Assembly::Instruction& emit(const Ts&... args) noexcept
        {
            static_assert(
                sizeof...(Ts) == Assembly::number_of_arguments(code),
                "Number of instructions arguments does not match number required!");
            return instructions_.emplace_back(Assembly::Instruction{code, args...});
        }

        [[nodiscard]] Assembly::MemoryIndex allocate_variable(const std::string& name) noexcept
        {
            return _allocate_new(names_, name);
        }

        [[nodiscard]] Assembly::MemoryIndex allocate_immediate(double value) noexcept
        {
            const auto index = _allocate_new(immediates_, value);
            immediate_values_.emplace_back(std::make_pair(value, index));
            return index;
        }

    private:
        template <typename Container, typename T = typename Container::value_type>
        [[nodiscard]] Assembly::MemoryIndex _allocate_new(Container& container, const T& value) noexcept
        {
            if (const auto it = container.find(value); it != container.end()) {
                return Assembly::make_memory_index(it->second);
            }
            const auto it = container.insert({value, _new_index()});
            return Assembly::make_memory_index(it.first->second);
        }

        std::size_t _new_index() noexcept
        {
            return current_index_++;
        }

        std::vector<Assembly::Instruction>& instructions_;
        Immediates& immediate_values_;

        std::size_t current_index_{2}; //the first two indecies are reserved

        std::unordered_map<std::string, std::size_t> names_;
        std::unordered_map<double, std::size_t> immediates_;
    };

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H