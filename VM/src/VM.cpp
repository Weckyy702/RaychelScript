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

#define __STDC_WANT_LIB_EXT1__ 1 //We want to use strerror_s if possible

#include "VM/VM.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <mutex>
#include <utility>
#include <string.h>

#include "RaychelCore/ScopedTimer.h"
#include "RaychelMath/equivalent.h"
#include "RaychelMath/math.h"

#define RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING 0

//FIXME: this needs to be enabled, but -Werror doesn't like this unknown pragma
//#pragma STDC FENV_ACCESS ON

#define END_REGULAR_HANDLER                                                                                                      \
    update_instruction_pointer(state, 1);                                                                                        \
    return VMErrorCode::ok

#define END_ARITHMETIC_HANDLER                                                                                                   \
    set_state_flags(state);                                                                                                      \
    state.check_fp_flag = true;                                                                                                  \
    END_REGULAR_HANDLER

namespace RaychelScript::VM {

    //helper functions

    template <std::floating_point T>
    static void set_instruction_pointer(VMState<T>& state, std::uint8_t instr_index) noexcept
    {
        state.instruction_pointer = state.instructions.begin() + instr_index;
    }

    template <std::floating_point T>
    static void update_instruction_pointer(VMState<T>& state, std::ptrdiff_t delta) noexcept
    {
        state.instruction_pointer += delta;
    }

    template <std::floating_point T>
    [[nodiscard]] static T& get_location(VMState<T>& state, std::uint8_t index) noexcept
    {
        //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): we have checked all accesses beforehand
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
        auto& result = get_result_location(state);

        result = lhs + rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_sub(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs - rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mul(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs * rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_div(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs / rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mag(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::abs(lhs);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_fac(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        if (lhs < 0 && Raychel::is_integer(lhs)) {
            return VMErrorCode::invalid_argument;
        }

        result = std::tgamma(lhs + 1);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_pow(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = std::pow(lhs, rhs);

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_inc(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs += rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_dec(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs -= rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_mas(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs *= rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_das(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs /= rhs;

        END_ARITHMETIC_HANDLER;
    }

    template <std::floating_point T>
    [[nodiscard]] static VMErrorCode handle_pas(VMState<T>& state, const Assembly::Instruction& instruction) noexcept
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs = std::pow(lhs, rhs);

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
            case Assembly::OpCode::inc:
                return handle_inc(state, instr);
            case Assembly::OpCode::dec:
                return handle_dec(state, instr);
            case Assembly::OpCode::mas:
                return handle_mas(state, instr);
            case Assembly::OpCode::das:
                return handle_das(state, instr);
            case Assembly::OpCode::pas:
                return handle_pas(state, instr);
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

    [[nodiscard]] bool instruction_indecies_in_range(const Assembly::Instruction& instruction, std::size_t size) noexcept
    {
        return std::cmp_less(instruction.data1(), size) && std::cmp_less(instruction.data2(), size);
    }

    template <std::floating_point T>
    bool instruction_access_in_range(const Assembly::Instruction& instruction, const VMState<T>& state)
    {
        using Assembly::OpCode;

        if (instruction.op_code() <= OpCode::cne) {
            return instruction_indecies_in_range(instruction, state.memory_size);
        }
        if (instruction.op_code() < OpCode::hlt) {
            return instruction_indecies_in_range(instruction, state.instructions.size());
        }
        return instruction.op_code() == OpCode::hlt;
    }

    static bool handle_fp_exceptions() noexcept
    {
        //NOLINTNEXTLINE(hicpp-signed-bitwise): we cannot change the STL spec :(
        return errno != 0 || fetestexcept(FE_ALL_EXCEPT & ~FE_INEXACT) != 0;
    }

    template <std::floating_point T>
    VMResult<T> execute(const VMData& data, const std::vector<T>& input_variables) noexcept
    {
#if RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        Raychel::ScopedTimer<std::chrono::microseconds> timer{"Execution time"};
#endif
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

        //Sanity check that the last instruction is a HLT instruction
        if (data.instructions.back().op_code() != Assembly::OpCode::hlt) {
            Logger::error("Last instruction is not a HLT instruction!\n");
            return VMErrorCode::last_instruction_not_hlt;
        }

        //Initialize state
        VMState<T> state{data.instructions, data.num_memory_locations};

        //Check all instruction indecies before execution so we don't have to check for overflows during execution
        for (const auto& instr : state.instructions) {
            if (!instruction_access_in_range(instr, state)) {
                return VMErrorCode::invalid_instruction_access;
            }
        }

        //initialize special variables
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
        while (!state.halt_flag) {
            if (const auto ec = execute_with_state(state); ec != VMErrorCode::ok) {
                dump_state(state, data);
                return ec;
            }
            if (state.check_fp_flag) {
                state.check_fp_flag = false;
                if (handle_fp_exceptions()) {
                    dump_state_fp_error(data, state);
                    return VMErrorCode::fp_exception;
                }
            }
        }

        return state;
    }

    namespace details {

        static void dump_instructions(const VMData& data, const auto instruction_pointer) noexcept
        {
            Logger::log("Instruction dump: (active instruction marked with '*'):\n");
            for (auto it = data.instructions.begin(); it != data.instructions.end(); it++) {
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

        static std::string_view errno_string() noexcept
        {
            static std::array<char, 255> output_buffer{};

#ifdef _WIN32
            strerror_s(output_buffer.data(), output_buffer.size(), errno);
            return output_buffer.data();
#else
            strerror_r(errno, output_buffer.data(), output_buffer.size());
            return output_buffer.data();
#endif
        }

        static std::string_view get_error_description() noexcept
        {
            if (errno != 0) {
                return errno_string();
            }

            if (std::fetestexcept(FE_DIVBYZERO) != 0) {
                return "Division by zero";
            }
            if (std::fetestexcept(FE_INVALID) != 0) {
                return "Argument is out of domain";
            }
            if (std::fetestexcept(FE_OVERFLOW) != 0) {
                return "Floating point overflow";
            }
            if (std::fetestexcept(FE_UNDERFLOW) != 0) {
                return "Floating point underflow";
            }
            return "Unknown error";
        }
    } // namespace details

    template <std::floating_point T>
    static void dump_state(const VMState<T>& state, const VMData& data) noexcept
    {
        if (data.num_memory_locations != state.memory_size) {
            return;
        }
        details::dump_instructions(data, state.instruction_pointer);
        details::dump_memory(data, state);
    }

    template <std::floating_point T>
    static void dump_state_fp_error(const VMData& data, const VMState<T>& state) noexcept
    {
        Logger::error("Floating-point error during execution: ", details::get_error_description(), "! Dumping state...\n");
        dump_state(state, data);
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
