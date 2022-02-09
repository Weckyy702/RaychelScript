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

#define __STDC_WANT_LIB_EXT1__ 1 //NOLINT(this enables strerror_s)

#include "VM/VM.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <mutex>
#include <utility>

#include "RaychelCore/ScopedTimer.h"
#include "RaychelMath/equivalent.h"
#include "RaychelMath/math.h"

#define RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING 0

#pragma STDC FENV_ACCESS ON

#define RAYCHELSCRIPT_VM_END_REGULAR_HANDLER state.instruction_pointer++

#define RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER                                                                                  \
    state.check_fp_flag = true;                                                                                                  \
    RAYCHELSCRIPT_VM_END_REGULAR_HANDLER

#define RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(name)                                                                          \
    template <std::floating_point T>                                                                                             \
    static void handle_##name(VMState<T>& state, [[maybe_unused]] const Assembly::Instruction& instruction) noexcept

namespace RaychelScript::VM {

    //helper functions

    template <std::floating_point T>
    static void set_error_state(VMState<T>& state, VMErrorCode error_code) noexcept
    {
        state.error_code = error_code;
        state.halt_flag = true;
    }

    template <std::floating_point T>
    static void set_instruction_pointer(VMState<T>& state, std::uint8_t instr_index) noexcept
    {
        state.instruction_pointer = state.instructions.begin() + instr_index;
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

    //Instruction handlers

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(mov)
    {
        const auto rhs = get_location(state, instruction.data1());
        auto& lhs = get_location(state, instruction.data2());

        lhs = rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(add)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs + rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(sub)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs - rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(mul)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs * rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(div)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs / rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(mag)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::abs(lhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(fac)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        if (lhs < 0 && Raychel::is_integer(lhs)) {
            return VMErrorCode::invalid_argument;
        }

        result = std::tgamma(lhs + 1);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(pow)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(inc)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs += rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(dec)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs -= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(mas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs *= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(das)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs /= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(pas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(jpz)
    {
        if (state.flag) {
            RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
        }

        set_instruction_pointer(state, instruction.data1());
        return VMErrorCode::ok;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(jmp)
    {
        set_instruction_pointer(state, instruction.data1());
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(hlt)
    {
        state.halt_flag = true;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(clt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs < rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(cgt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs > rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(ceq)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_INSTRUCTION_HANDLER_FUNC(cne)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = !Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    template <std::floating_point T>
    static void execute_next_instruction(VMState<T>& state) noexcept
    {
        const auto& instr = *state.instruction_pointer;

        switch (instr.op_code()) {
            case Assembly::OpCode::mov:
                handle_mov(state, instr);
                break;
            case Assembly::OpCode::add:
                handle_add(state, instr);
                break;
            case Assembly::OpCode::sub:
                handle_sub(state, instr);
                break;
            case Assembly::OpCode::mul:
                handle_mul(state, instr);
                break;
            case Assembly::OpCode::div:
                handle_div(state, instr);
                break;
            case Assembly::OpCode::mag:
                handle_mag(state, instr);
                break;
            case Assembly::OpCode::fac:
                handle_fac(state, instr);
                break;
            case Assembly::OpCode::pow:
                handle_pow(state, instr);
                break;
            case Assembly::OpCode::inc:
                handle_inc(state, instr);
                break;
            case Assembly::OpCode::dec:
                handle_dec(state, instr);
                break;
            case Assembly::OpCode::mas:
                handle_mas(state, instr);
                break;
            case Assembly::OpCode::das:
                handle_das(state, instr);
                break;
            case Assembly::OpCode::pas:
                handle_pas(state, instr);
                break;
            case Assembly::OpCode::jpz:
                handle_jpz(state, instr);
                break;
            case Assembly::OpCode::jmp:
                handle_jmp(state, instr);
                break;
            case Assembly::OpCode::hlt:
                handle_hlt(state, instr);
                break;
            case Assembly::OpCode::clt:
                handle_clt(state, instr);
                break;
            case Assembly::OpCode::cgt:
                handle_cgt(state, instr);
                break;
            case Assembly::OpCode::ceq:
                handle_ceq(state, instr);
                break;
            case Assembly::OpCode::cne:
                handle_cne(state, instr);
                break;
            default:
                set_error_state(state, VMErrorCode::unknow_op_code);
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

    [[nodiscard]] static bool has_fp_exception() noexcept
    {
        //NOLINTNEXTLINE(hicpp-signed-bitwise): we cannot change the STL spec :(
        return errno != 0 || fetestexcept(FE_ALL_EXCEPT & ~FE_INEXACT) != 0;
    }

    template <std::floating_point T>
    VMResult<T> execute(const VMData& data, const std::vector<T>& input_variables) noexcept
    {
#if RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        const auto start = std::chrono::high_resolution_clock::now();
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

        feclearexcept(FE_ALL_EXCEPT); //clear the FP exception flags
        //main execution loop
        while (!state.halt_flag) {
            execute_next_instruction(state);
            if (state.check_fp_flag) {
                state.check_fp_flag = false;
                if (has_fp_exception()) {
                    dump_state_fp_error(data, state);
                    return VMErrorCode::fp_exception;
                }
            }
        }

#if RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        const auto end = std::chrono::high_resolution_clock::now();

        Logger::info(std::chrono::duration_cast<std::chrono::nanos>(end - start).count(), "us\n");
#endif

        if (state.error_code != VMErrorCode::ok) {
            return state.error_code;
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
            (void)execute<float>({}, {});
            (void)execute<double>({}, {});
            (void)execute<long double>({}, {});
        }
    } // namespace details

} //namespace RaychelScript::VM
