/**
* \file VM.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for VM class
* \date 2022-06-07
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
#define RAYCHELSCRIPT_VM_USE_COMPUTED_GOTO 0

#pragma STDC FENV_ACCESS ON

#include "VM/VM.h"

#include "RaychelMath/equivalent.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <variant>

#define RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(code)                                                                        \
    static void handle_##code(VMState& state, const auto& instruction) noexcept

#define RAYCHELSCRIPT_VM_END_REGULAR_HANDLER ++state.instruction_pointer;

#define RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER                                                                                  \
    state.check_fp_flag = true;                                                                                                  \
    RAYCHELSCRIPT_VM_END_REGULAR_HANDLER

namespace RaychelScript::VM {

    void set_error_state(VMState& state, VMErrorCode error_code) noexcept
    {
        state.error_code = error_code;
        state.halt_flag = true;
    }

    void set_instruction_pointer(VMState& state, std::uint8_t instr_index) noexcept
    {
        state.instruction_pointer = state.instructions.begin() + instr_index;
    }

    [[nodiscard]] double& get_location(VMState& state, std::uint8_t index) noexcept
    {
        //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): we have checked all accesses beforehand
        return state.memory[index];
    }

    [[nodiscard]] double& get_result_location(VMState& state) noexcept
    {
        return get_location(state, 0);
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(mov)
    {
        const auto rhs = get_location(state, instruction.data1());
        auto& lhs = get_location(state, instruction.data2());

        lhs = rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(add)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs + rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(sub)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs - rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(mul)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs * rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(div)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = lhs / rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(mag)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::abs(lhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(fac)
    {
        const auto lhs = get_location(state, instruction.data1());
        auto& result = get_result_location(state);

        result = std::tgamma(lhs + 1);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(pow)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());
        auto& result = get_result_location(state);

        result = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(inc)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs += rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(dec)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs -= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(mas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs *= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(das)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs /= rhs;

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(pas)
    {
        auto& lhs = get_location(state, instruction.data1());
        const auto& rhs = get_location(state, instruction.data2());

        lhs = std::pow(lhs, rhs);

        RAYCHELSCRIPT_VM_END_ARITHMETIC_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(jpz)
    {
        if (state.flag) {
            state.flag = false;
            RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
            return;
        }

        set_instruction_pointer(state, instruction.data1());
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(jmp)
    {
        set_instruction_pointer(state, instruction.data1());
    }

#if !RAYCHELSCRIPT_VM_USE_COMPUTED_GOTO
    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(hlt)
    {
        (void)instruction;
        state.halt_flag = true;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }
#endif

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(clt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs < rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(cgt)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = lhs > rhs;
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(ceq)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

    RAYCHELSCRIPT_VM_DEFINE_INSTRUCTION_HANDLER(cne)
    {
        const auto lhs = get_location(state, instruction.data1());
        const auto rhs = get_location(state, instruction.data2());

        state.flag = !Raychel::equivalent(lhs, rhs);
        RAYCHELSCRIPT_VM_END_REGULAR_HANDLER;
    }

#if !RAYCHELSCRIPT_VM_USE_COMPUTED_GOTO
    [[maybe_unused]] static void execute_next_instruction(VMState& state) noexcept
    {
        constexpr std::array dispatch_table{handle_mov, handle_add, handle_sub, handle_mul, handle_div, handle_mag, handle_fac,
                                            handle_pow, handle_inc, handle_dec, handle_mas, handle_das, handle_pas, handle_clt,
                                            handle_cgt, handle_ceq, handle_cne, handle_jpz, handle_jmp, handle_hlt};

        const auto& instruction = *state.instruction_pointer;

        if (instruction.op_code() >= Assembly::OpCode::num_op_codes) {
            set_error_state(state, VMErrorCode::unknow_op_code);
            return;
        }
        dispatch_table[static_cast<std::size_t>(instruction.op_code())](state, instruction);
    }
#endif

    namespace details {
        [[nodiscard]] static bool
        instruction_indecies_in_range(const Assembly::Instruction& instruction, std::size_t size) noexcept
        {
            return std::cmp_less(instruction.data1(), size) && std::cmp_less(instruction.data2(), size);
        }

        [[maybe_unused]] [[nodiscard]] static bool
        instruction_access_in_range(const Assembly::Instruction& instruction, const VMState& state)
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

#if !RAYCHELSCRIPT_VM_USE_COMPUTED_GOTO
        [[maybe_unused]] [[nodiscard]] static bool has_fp_exception() noexcept
        {
            //NOLINTNEXTLINE(hicpp-signed-bitwise): we cannot change the STL spec :(
            return errno != 0 || fetestexcept(FE_ALL_EXCEPT & ~FE_INEXACT) != 0;
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

        [[maybe_unused]] static std::string_view get_error_description() noexcept
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

        [[maybe_unused]] static void
        dump_state_fp_error([[maybe_unused]] const VMData& data, [[maybe_unused]] const VMState& state) noexcept
        {
    #if RAYCHELSCRIPT_VM_ENABLE_FP_EXCEPTION_DUMP
            Logger::error("Floating-point error during execution: ", get_error_description(), "! Dumping state...\n");
            dump_state(state, data);
    #endif
        }
#endif
    } // namespace details

    VMResult execute(const VMData& data, const std::vector<double>& input_variables) noexcept
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
        VMState state{data.instructions, data.num_memory_locations};

        //Check all instruction indecies before execution so we don't have to check for out-of-bounds accesses during execution
        for (const auto& instr : state.instructions) {
            if (!details::instruction_access_in_range(instr, state)) {
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
            get_location(state, index.value()) = value;
        }

#if !RAYCHELSCRIPT_VM_USE_COMPUTED_GOTO
        //NOLINTNEXTLINE(hicpp-signed-bitwise)
        feclearexcept(FE_ALL_EXCEPT); //clear the FP exception flags

        //main execution loop
        while (!state.halt_flag) {
            execute_next_instruction(state);

            if (state.check_fp_flag) {
                state.check_fp_flag = false;
                if (details::has_fp_exception()) {
                    details::dump_state_fp_error(data, state);
                    return VMErrorCode::fp_exception;
                }
            }
        }
#else
        {
            constexpr std::array jump_table{&&mov, &&add, &&sub, &&mul, &&div, &&mag, &&fac, &&pow, &&inc, &&dec,           &&mas,
                                            &&das, &&pas, &&clt, &&cgt, &&ceq, &&cne, &&jpz, &&jmp, &&hlt, &&invalid_opcode};
            const auto next_instruction = [&] {
                if (state.instruction_pointer->op_code() >= Assembly::OpCode::num_op_codes) {
                    return jump_table.back();
                }
                return jump_table[static_cast<std::size_t>(state.instruction_pointer->op_code())];
            };

            goto* next_instruction();

        mov:
            handle_mov(state, *state.instruction_pointer);
            goto* next_instruction();
        add:
            handle_add(state, *state.instruction_pointer);
            goto* next_instruction();
        sub:
            handle_sub(state, *state.instruction_pointer);
            goto* next_instruction();
        mul:
            handle_mul(state, *state.instruction_pointer);
            goto* next_instruction();
        div:
            handle_div(state, *state.instruction_pointer);
            goto* next_instruction();
        mag:
            handle_mag(state, *state.instruction_pointer);
            goto* next_instruction();
        fac:
            handle_fac(state, *state.instruction_pointer);
            goto* next_instruction();
        pow:
            handle_pow(state, *state.instruction_pointer);
            goto* next_instruction();
        inc:
            handle_inc(state, *state.instruction_pointer);
            goto* next_instruction();
        dec:
            handle_dec(state, *state.instruction_pointer);
            goto* next_instruction();
        mas:
            handle_mas(state, *state.instruction_pointer);
            goto* next_instruction();
        das:
            handle_das(state, *state.instruction_pointer);
            goto* next_instruction();
        pas:
            handle_pas(state, *state.instruction_pointer);
            goto* next_instruction();
        clt:
            handle_clt(state, *state.instruction_pointer);
            goto* next_instruction();
        cgt:
            handle_cgt(state, *state.instruction_pointer);
            goto* next_instruction();
        ceq:
            handle_ceq(state, *state.instruction_pointer);
            goto* next_instruction();
        cne:
            handle_cne(state, *state.instruction_pointer);
            goto* next_instruction();
        jpz:
            handle_jpz(state, *state.instruction_pointer);
            goto* next_instruction();
        jmp:
            handle_jmp(state, *state.instruction_pointer);
            goto* next_instruction();
        invalid_opcode:
            return VMErrorCode::unknow_op_code;
        hlt:
        {
            //nothing to do here :)
        }
        }
#endif

#if RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        const auto end = std::chrono::high_resolution_clock::now();

        Logger::info(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(), "ns\n");
#endif

        if (state.error_code != VMErrorCode::ok) {
            return state.error_code;
        }

        return state;
    }

    namespace details {

        static void dump_instructions(const VMState& state, const auto instruction_pointer) noexcept
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

        static bool dump_value_with_maybe_name(std::size_t index, double value, const auto& container) noexcept
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

        static void dump_memory(const VMData& data, const VMState& state) noexcept
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

    void dump_state(const VMState& state, const VMData& data) noexcept
    {
        if (data.num_memory_locations != state.memory_size) {
            return;
        }
        details::dump_instructions(state, state.instruction_pointer);
        details::dump_memory(data, state);
    }

} //namespace RaychelScript::VM
