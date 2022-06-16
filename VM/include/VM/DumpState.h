/**
* \file DumpState.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for state dumping function
* \date 2022-02-10
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
#ifndef RAYCHELSCRIPT_VM_DUMP_STATE_H
#define RAYCHELSCRIPT_VM_DUMP_STATE_H

#include "VMState.h"
#include "shared/VM/VMData.h"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <utility>

namespace RaychelScript::VM {

    namespace details {

        template <std::floating_point T>
        static void dump_instructions(const VMState<T>& state, const auto instruction_pointer) noexcept
        {
            Logger::log("Instruction dump: (active instruction marked with '*'):\n");
            for (auto it = state.instructions.begin(); it != state.instructions.end(); it++) {
                if (it == std::prev(instruction_pointer)) {
                    Logger::log('*');
                } else {
                    Logger::log(' ');
                }
                Logger::log(*it, '\n');
            }
        }

        template <typename Container, std::floating_point T = typename Container::value_type>
        static bool dump_value_with_maybe_name(std::size_t index, T value, const Container& container) noexcept
        {
            const auto it = std::find_if(container.begin(), container.end(), [&](const auto& descriptor) {
                return std::cmp_equal(descriptor.second.value(), index);
            });
            if (it == container.end()) {
                return false;
            }
            Logger::log(it->first, " -> ", value, '\n');
            return true;
        }

        template <std::floating_point T>
        static void dump_memory(const VMData& data, const VMState<T>& state) noexcept
        {
            Logger::log("$A -> ", state.memory.at(0), '\n');
            for (std::size_t i = 1; i < state.memory_size; i++) {
                if (dump_value_with_maybe_name(i, state.memory.at(i), data.config_block.input_identifiers)) {
                    continue;
                }
                if (dump_value_with_maybe_name(i, state.memory.at(i), data.config_block.output_identifiers)) {
                    continue;
                }
                Logger::log('$', i, " -> ", state.memory.at(i), '\n');
            }
        }
    } // namespace details

    template <std::floating_point T>
    void dump_state(const VMState<T>& state, const VMData& data) noexcept
    {
        if (data.num_memory_locations != state.memory_size) {
            return;
        }
        details::dump_instructions(state, state.instruction_pointer);
        details::dump_memory(data, state);
    }

} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_DUMP_STATE_H
