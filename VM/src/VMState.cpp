/**
* \file VMState.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for VMState class
* \date 2022-06-25
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

#include "VM/VMState.h"

namespace RaychelScript::VM {
    std::vector<double> get_output_values(const VMState& state, const VM::VMData& data) noexcept
    {
        if (std::cmp_not_equal(state.memory.size(), data.num_memory_locations)) {
            return {};
        }

        std::vector<double> result;
        result.reserve(data.config_block.output_identifiers.size());
        for ([[maybe_unused]] const auto& [_, address] : data.config_block.output_identifiers) {
            result.emplace_back(state.memory.at(address.value()));
        }

        return result;
    }

    std::vector<std::pair<std::string, double>> get_output_variables(const VMState& state, const VM::VMData& data) noexcept
    {
        if (std::cmp_not_equal(state.memory_size, data.num_memory_locations)) {
            return {};
        }

        std::vector<std::pair<std::string, double>> result;
        result.reserve(data.config_block.output_identifiers.size());
        for (const auto& [name, address] : data.config_block.output_identifiers) {
            result.emplace_back(name, state.memory.at(address.value()));
        }

        return result;
    }

} //namespace RaychelScript::VM
