/**
* \file InstructionHandlers.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for instruction handler functions
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
#ifndef RAYCHELSCRIPT_VM_INSTRUCTION_HANDLERS_H
#define RAYCHELSCRIPT_VM_INSTRUCTION_HANDLERS_H

#include "VMState.h"
#include "shared/rasm/Instruction.h"

#include <concepts>

#include "RaychelMath/equivalent.h"

#define RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(code)                                                                        \
    template <>                                                                                                                  \
    struct InstructionHandler<code>                                                                                              \
    {                                                                                                                            \
        template <std::floating_point T>                                                                                         \
        static constexpr void handle(VMState<T>& state, const Assembly::Instruction& instruction)

#define RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER                                                                                 \
    }                                                                                                                            \
    ;

#define RAYCHELSCRIPT_VM_END_REGULAR_HANDLER state.instruction_pointer++

#define RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER                                                                                  \
    state.check_fp_flag = true;                                                                                                  \
    RAYCHELSCRIPT_VM_END_REGULAR_HANDLER

namespace RaychelScript::VM {

    template <Assembly::OpCode code>
    struct InstructionHandler
    {};

    template <std::floating_point T>
    void set_error_state(VMState<T>& state, VMErrorCode error_code) noexcept
    {
        state.error_code = error_code;
        state.halt_flag = true;
    }

    template <std::floating_point T>
    void set_instruction_pointer(VMState<T>& state, std::uint8_t instr_index) noexcept
    {
        state.instruction_pointer = state.instructions.begin() + instr_index;
    }

    template <std::floating_point T>
    [[nodiscard]] T& get_result_location(VMState<T>& state) noexcept
    {
        return get_location(state, 0);
    }

    template <std::floating_point T>
    [[nodiscard]] T& get_location(VMState<T>& state, std::uint8_t index) noexcept
    {
        //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): we have checked all accesses beforehand
        return state.memory[index];
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::mov)
    {
        const auto rhs = get_location(state, instruction.data1());
        auto& lhs = get_location(state, instruction.data2());

        lhs = rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::add)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs + rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::sub)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs - rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::mul)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs * rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::div)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs / rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::mag)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::abs(lhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::fac)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::tgamma(lhs + 1);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::pow)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::inc)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs += rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::dec)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs -= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::mas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs *= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::das)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs /= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::pas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::jpz)
    {
        if (state.flag) {
            state.flag = false;
            RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
        }

        set_instruction_pointer(state, instruction.data1());
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::jmp)
    {
        set_instruction_pointer(state, instruction.data1());
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::hlt)
    {
        (void)instruction;
        state.halt_flag = true;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::clt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs < rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::cgt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs > rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::ceq)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(Assembly::OpCode::cne)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = !Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
    RAYCHELSCRIPT_VM_END_INSTRUCTION_HANDLER
} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_INSTRUCTION_HANDLERS_H