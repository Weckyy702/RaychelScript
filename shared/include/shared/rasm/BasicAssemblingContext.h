/**
* \file BasicAssemblingContext.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for BasicAssemblingContext class
* \date 2022-02-13
* 
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_BASIC_ASSEMBLING_CONTEXT_H
#define RAYCHELSCRIPT_ASSEMBLY_BASIC_ASSEMBLING_CONTEXT_H

#include "MemoryIndex.h"

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "RaychelCore/ClassMacros.h"

namespace RaychelScript::Assembly {

    class BasicAssemblingContext
    {
        struct ScopeData
        {
            std::vector<std::string> names;
        };

        RAYCHEL_MAKE_NONCOPY_NONMOVE(BasicAssemblingContext)
    public:
        BasicAssemblingContext() = default;

        BasicAssemblingContext(std::size_t register_offset) : current_index_{register_offset}
        {}

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
            allocate_immediate_internal(did_insert, index, value);
            return index;
        }

        /**
        * \brief Return the index to the last instruction
        */
        [[nodiscard]] virtual std::size_t instruction_index() const noexcept=0;

        [[nodiscard]] auto number_of_memory_locations() const noexcept
        {
            RAYCHEL_ASSERT(std::cmp_less_equal(current_index_, std::numeric_limits<std::uint8_t>::max()));
            return static_cast<std::uint8_t>(current_index_);
        }

        virtual ~BasicAssemblingContext() = default;

    protected:
        [[nodiscard]] auto& scopes() noexcept
        {
            return scopes_;
        }

        [[nodiscard]] auto& names() noexcept
        {
            return names_;
        }

        [[nodiscard]] Assembly::MemoryIndex new_index() noexcept
        {
            return Assembly::make_memory_index(current_index_++);
        }

        virtual void allocate_immediate_internal(bool did_insert, MemoryIndex index, double value) noexcept
        {
            (void)did_insert;
            (void)index;
            (void)value;
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
            const auto it = container.insert({value, new_index()});
            return {true, it.first->second};
        }

        std::size_t current_index_{0};

        std::unordered_map<std::string, MemoryIndex> names_;
        std::unordered_map<double, MemoryIndex> immediates_;

        std::stack<ScopeData, std::vector<ScopeData>> scopes_;
    };

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_BASIC_ASSEMBLING_CONTEXT_H