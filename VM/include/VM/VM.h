/**
* \file VM.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VM class
* \date 2021-12-27
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
#ifndef RAYCHELSCRIPT_VM_H
#define RAYCHELSCRIPT_VM_H

#include "VMErrorCode.h"
#include "VMState.h"

#include "RaychelCore/Finally.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <span>
#include <variant>

namespace RaychelScript::VM {

    [[nodiscard]] VMErrorCode execute(
        const VMData& data, std::span<const double> input_variables, std::span<double> output_values, std::size_t stack_size,
        std::size_t memory_size, std::pmr::memory_resource* memory_resource) noexcept;

    namespace details {

        void debug_log_vm_memory(const auto& buf, std::size_t stack_size)
        {
#if 0
            std::cout << std::hex << std::setfill('0');
            std::size_t index_offset{};
            std::size_t object_index{};
            for (std::size_t i{}; i < buf.size(); i += 8U) {
                std::cout << "\033[0m" << std::dec << ++object_index - index_offset << ": " << std::hex;
                if (i < stack_size * sizeof(VMState::CallFrame)) {
                    std::cout << "\033[32m";
                    const auto frame = *std::launder(reinterpret_cast<const VMState::CallFrame*>(&buf[i]));
                    auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(VMState::CallFrame)>>(frame);
                    std::size_t index{};
                    for (const auto& byte : bytes) {
                        std::cout << "0x" << std::setw(2) << static_cast<std::uint32_t>(byte) << ' ';
                        if (++index == 7)
                            std::cout << "| ";
                    }
                    std::cout << "= CallFrame{ip=0x" << std::setw(16)
                              << reinterpret_cast<const uintptr_t>(frame.instruction_pointer.base()) << ", size=0x" << frame.size
                              << '}';
                    i += 8U;
                } else {
                    index_offset = stack_size;
                    std::cout << "\033[35m";
                    const auto value = *std::launder(reinterpret_cast<const double*>(&buf[i]));
                    auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(double)>>(value);
                    for (const auto& byte : std::ranges::reverse_view(bytes)) {
                        std::cout << "0x" << std::setw(2) << static_cast<std::uint32_t>(byte) << ' ';
                    }
                    std::cout << "= " << value;
                }
                std::cout << '\n';
            }
            std::cout << "\033[0m" << std::dec;
#else
            (void)buf;
            (void)stack_size;
#endif
        }

        template <typename OutputContainer, std::size_t stack_size, std::size_t memory_size, typename Init>
        std::variant<VMErrorCode, OutputContainer>
        do_execute(const VMData& data, std::span<const double> input_values, Init&& init)
        {
            std::array<std::byte, stack_size * sizeof(VMState::CallFrame) + memory_size * sizeof(double)> buf{};
            std::pmr::monotonic_buffer_resource resource{buf.data(), buf.size(), std::pmr::null_memory_resource()};
            OutputContainer outputs{};
            init(outputs);

            [[maybe_unused]] Raychel::Finally _{[&buf] { debug_log_vm_memory(buf, stack_size); }};

            if (const auto ec = execute(data, input_values, outputs, stack_size, memory_size, &resource); ec != VMErrorCode::ok)
                return ec;

            return outputs;
        }

        template <std::size_t NumOutputs, std::size_t stack_size, std::size_t memory_size>
        struct DoExecute
        {
            auto operator()(const VMData& data, std::span<const double> input_values) const noexcept
            {
                return do_execute<std::array<double, NumOutputs>, stack_size, memory_size>(data, input_values, [](auto&) {});
            }
        };

        template <::std::size_t stack_size, ::std::size_t memory_size>
        struct DoExecute<std::dynamic_extent, stack_size, memory_size>
        {
            auto operator()(const VMData& data, std::span<const double> input_values) const noexcept
            {
                return do_execute<std::vector<double>, stack_size, memory_size>(
                    data, input_values, [cap = data.num_output_identifiers](auto& v) { v.resize(cap); });
            }
        };
    } // namespace details

    template <std::size_t NumOutputs, std::size_t stack_size = 128U, std::size_t memory_size = 1'024U>
    [[nodiscard]] auto execute(const VMData& data, std::span<const double> input_values) noexcept
    {
        return details::DoExecute<NumOutputs, stack_size, memory_size>{}(data, input_values);
    }

} // namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_H
