/**
* \file VMState.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VMState class
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
#ifndef RAYCHELSCRIPT_VM_STATE_H
#define RAYCHELSCRIPT_VM_STATE_H

#include "VMErrorCode.h"
#include "shared/VM/VMData.h"

#include <array>
#include <concepts>
#include <cstdint>
#include <memory_resource>
#include <stack>
#include <vector>

namespace RaychelScript::VM {

    template <typename T>
    using DynamicArray = std::pmr::vector<T>;

    namespace details {
        //Clang does not fully support ranges yet (at least libstdc++'s) so we need our own
        // clang-format off
        template <typename Container, typename It>
        concept RangeConstructible = requires(Container c)
        {
            { c.begin() } -> std::same_as<It>;
            { c.end() } -> std::same_as<It>;
        };
        // clang-format on
        template <typename Ptr>
        struct Range
        {
            template <RangeConstructible<Ptr> Container>
            explicit constexpr Range(Container& c) : begin{c.begin()}, end{c.end()}
            {}

            template <RangeConstructible<Ptr> Container>
            explicit constexpr Range(const Container& c) : begin{c.begin()}, end{c.end()}
            {}

            //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            constexpr Range(Ptr _begin, Ptr _end) : begin{_begin}, end{_end}
            {}

            Ptr begin{};
            Ptr end{};
        };

        template <typename Container>
        Range(Container&) -> Range<typename Container::iterator>;

        template <typename Container>
        Range(const Container&) -> Range<typename Container::const_iterator>;

    } // namespace details

    struct VMState
    {
        struct CallFrame;
        using InstructionPointer = std::vector<Assembly::Instruction>::const_iterator;
        using StackPointer = DynamicArray<double>::iterator;
        using FramePointer = CallFrame*;

        struct CallFrame
        {
            InstructionPointer instruction_pointer{};
            std::ptrdiff_t size{};
        };

        explicit VMState(details::Range<StackPointer> memory, details::Range<FramePointer> stack, const VMData& data) noexcept;

        FramePointer frame_pointer;
        StackPointer stack_pointer;

        VMErrorCode error{};
        bool halt_flag : 1 {false};
        bool flag : 1 {false};

        std::size_t call_depth{};
        std::size_t instruction_count{};
        std::size_t function_call_count{};

        //NOLINTBEGIN(misc-misplaced-const)
        const FramePointer beginning_of_stack;
        const FramePointer end_of_stack;
        const StackPointer end_of_memory;
        //NOLINTEND(misc-misplaced-const)

        const VMData& data;
    };

    std::vector<double> get_output_values(const VMState& state, const VM::VMData& data) noexcept;

    void dump_state(const VMState& state, const VMData& data) noexcept;
} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_STATE_H
