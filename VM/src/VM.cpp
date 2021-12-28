/**
* \file VM.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for VM class
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

#include "VM/VM.h"

#include <cmath>
#include <utility>
#include "RaychelMath/equivalent.h"
#include "RaychelMath/math.h"
#include "RaychelCore/ScopedTimer.h"

#define END_REGULAR_HANDLER                                                                                                      \
    update_instruction_pointer(state, 1);                                                                                        \
    return VMErrorCode::ok

#define END_ARITHMETIC_HANDLER                                                                                                   \
    set_state_flags(state);                                                                                                      \
    END_REGULAR_HANDLER

namespace RaychelScript::VM {

    //helper functions

    template <std::floating_point T>
    static void set_instruction_pointer(VMState<T>& state, std::uint8_t instr_index) noexcept
    {
        RAYCHEL_ASSERT(std::cmp_less(instr_index, state.instructions.size()));
        state.instruction_pointer = state.instructions.begin() + instr_index;
    }

    template <std::floating_point T>
    static void update_instruction_pointer(VMState<T>& state, std::size_t delta) noexcept
    {
        RAYCHEL_ASSERT(std::cmp_less_equal(delta, std::distance(state.instruction_pointer, state.instructions.end())));
        state.instruction_pointer += static_cast<std::ptrdiff_t>(delta);
    }

    template <std::floating_point T>
    [[nodiscard]] static T& get_location(VMState<T>& state, std::uint8_t index) noexcept
    {
        RAYCHEL_ASSERT(index < state.memory.size());
        return state.memory[index];
    }

    template <std::floating_point T>
    [[nodiscard]] static T& get_result_location(VMState<T>& state) noexcept
    {
        return get_location(state, 0);
    }

    template <std::floating_point T>
    void set_state_flags(VMState<T>& state) noexcept
    {
        //TODO: implement arithmetic flags. We don't seem to need any right now
        (void)state;
    }

    //Instruction handlers

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mov(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto rhs = get_location(state, instruction.data1());
        auto& lhs = get_location(state, instruction.data2());

        lhs = rhs;

        END_REGULAR_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_add(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result_location = get_result_location(state);

        result_location = lhs + rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_sub(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result_location = get_result_location(state);

        result_location = lhs - rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mul(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result_location = get_result_location(state);

        result_location = lhs * rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_div(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result_location = get_result_location(state);

        if (Raychel::equivalent<T>(rhs, 0)) {
            return VMErrorCode::divide_by_zero;
        }

        result_location = lhs / rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mag(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result_location = get_result_location(state);

        result_location = std::abs(lhs);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_fac(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result_location = get_result_location(state);

        if (lhs < 0 && Raychel::is_integer(lhs)) {
            return VMErrorCode::invalid_argument;
        }

        result_location = std::tgamma(lhs + 1);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_pow(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result_location = get_result_location(state);

        result_location = std::pow(lhs, rhs);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_jpz(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        if (state.flag) {
            END_REGULAR_HANDLER;
        }

        set_instruction_pointer(state, instruction.data1());
        return VMErrorCode::ok;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_jmp(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        set_instruction_pointer(state, instruction.data1());
        return VMErrorCode::ok;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode
    handle_hlt(VMState<T>& state, [[maybe_unused]] const Assembly::Instruction& instruction) noexcept
    {
        state.halt_flag = true;
        return VMErrorCode::ok;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_clt(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs < rhs;

        END_REGULAR_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_cgt(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs > rhs;

        END_REGULAR_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_ceq(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = Raychel::equivalent(lhs, rhs);

        END_REGULAR_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_cne(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = !Raychel::equivalent(lhs, rhs);

        END_REGULAR_HANDLER;
    }

#define TRY(expression)                                                                                                          \
    if (const auto ec = (expression); ec != VMErrorCode::ok) {                                                                   \
        return ec;                                                                                                               \
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode execute_with_state(VMState<T>& state) noexcept
    {
        const auto& instr = *state.instruction_pointer;

        switch (instr.op_code()) {
            case Assembly::OpCode::mov:
                return handle_mov(state, instr);
            case Assembly::OpCode::add:
                return handle_add(state, instr);
            case Assembly::OpCode::sub:
                return handle_sub(state, instr);
            case Assembly::OpCode::mul:
                return handle_mul(state, instr);
            case Assembly::OpCode::div:
                return handle_div(state, instr);
            case Assembly::OpCode::mag:
                return handle_mag(state, instr);
            case Assembly::OpCode::fac:
                return handle_fac(state, instr);
            case Assembly::OpCode::pow:
                return handle_pow(state, instr);
            case Assembly::OpCode::jpz:
                return handle_jpz(state, instr);
            case Assembly::OpCode::jmp:
                return handle_jmp(state, instr);
            case Assembly::OpCode::hlt:
                return handle_hlt(state, instr);
            case Assembly::OpCode::clt:
                return handle_clt(state, instr);
            case Assembly::OpCode::cgt:
                return handle_cgt(state, instr);
            case Assembly::OpCode::ceq:
                return handle_ceq(state, instr);
            case Assembly::OpCode::cne:
                return handle_cne(state, instr);
            default:
                return VMErrorCode::unknow_op_code;
        }
    }

    template <std::floating_point T>
    VMResult<T> execute(const Assembly::VMData& data, const std::vector<T>& input_variables) noexcept
    {
        Raychel::ScopedTimer<std::chrono::microseconds> timer{"Execution time"};
        //Check for the correct number of input identifiers
        if (input_variables.size() != data.config_block.input_identifiers.size()) {
            Logger::error(
                "Mismatched number of input identifiers! Expected ",
                data.config_block.input_identifiers.size(),
                ", got ",
                input_variables.size(),
                '\n');
            return VMErrorCode::mismatched_input_identifiers;
        }

        //Initialize state
        VMState<T> state{data.instructions, data.num_memory_locations};

        //initialize input variables
        {
            auto it = input_variables.begin();
            for (const auto& [_, address] : data.config_block.input_identifiers) {
                get_location(state, address.value()) = *(it++);
            }
        }

        for (const auto& [value, index] : data.immediate_values) {
            get_location(state, index.value()) = static_cast<T>(value);
        }

        //main execution loop
        while (state.instruction_pointer != state.instructions.end()) {
            if (state.halt_flag) {
                break;
            }
            TRY(execute_with_state(state));
        }

        return state;
    }

    namespace details {
        void _instantiate_vm_entry_points() noexcept
        {
            execute<float>({}, {});
            execute<double>({}, {});
            execute<long double>({}, {});
        }
    } // namespace details

} //namespace RaychelScript::VM