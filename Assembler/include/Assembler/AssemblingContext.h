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

#include "shared/rasm/BasicAssemblingContext.h"
#include "shared/rasm/Instruction.h"

namespace RaychelScript::Assembler {

    class AssemblingContext final : public Assembly::BasicAssemblingContext
    {

        using Immediates = std::vector<std::pair<double, Assembly::MemoryIndex>>;

    public:
        explicit AssemblingContext(std::vector<Assembly::Instruction>& instructions, Immediates& immediate_values)
            : Assembly::BasicAssemblingContext{1}, instructions_{instructions}, immediate_values_{immediate_values}
        {}

        /**
        * \brief Return the index for the reserved A register
        */
        [[nodiscard]] static constexpr Assembly::MemoryIndex result_index() noexcept
        {
            return Assembly::make_memory_index(0);
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
        * \brief Return the index to the last instruction
        */
        [[nodiscard]] std::size_t instruction_index() const noexcept override
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

            auto idx = new_index();
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

        void allocate_immediate_internal(bool did_insert, Assembly::MemoryIndex index, double value) noexcept override
        {
            if (did_insert)
                immediate_values_.emplace_back(std::make_pair(value, index));
        }

    private:
        std::vector<Assembly::Instruction>& instructions_;
        Immediates& immediate_values_;
        std::vector<std::pair<bool, Assembly::MemoryIndex>> intermediates_;
    };

} //namespace RaychelScript::Assembler

#endif //!RAYCHELSCRIPT_ASSEMBLER_CONTEXT_H