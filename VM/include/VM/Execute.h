/**
* \file Execute.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Execute class
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
#ifndef RAYCHELSCRIPT_VM_EXECUTE_H
#define RAYCHELSCRIPT_VM_EXECUTE_H

#pragma STDC FENV_ACCESS ON

#include "InstructionHandlers.h"

#include <cerrno>
#include <cfenv>
#include <variant>

namespace RaychelScript::VM {

    template <std::floating_point T>
    void execute_next_instruction(VMState<T>& state) noexcept
    {
        const auto& instruction = *state.instruction_pointer;

        switch (instruction.op_code()) {
            case Assembly::OpCode::mov:
                InstructionHandler<Assembly::OpCode::mov>::handle(state, instruction);
                return;
            case Assembly::OpCode::add:
                InstructionHandler<Assembly::OpCode::add>::handle(state, instruction);
                return;
            case Assembly::OpCode::sub:
                InstructionHandler<Assembly::OpCode::sub>::handle(state, instruction);
                return;
            case Assembly::OpCode::mul:
                InstructionHandler<Assembly::OpCode::mul>::handle(state, instruction);
                return;
            case Assembly::OpCode::div:
                InstructionHandler<Assembly::OpCode::div>::handle(state, instruction);
                return;
            case Assembly::OpCode::mag:
                InstructionHandler<Assembly::OpCode::mag>::handle(state, instruction);
                return;
            case Assembly::OpCode::fac:
                InstructionHandler<Assembly::OpCode::fac>::handle(state, instruction);
                return;
            case Assembly::OpCode::pow:
                InstructionHandler<Assembly::OpCode::pow>::handle(state, instruction);
                return;
            case Assembly::OpCode::inc:
                InstructionHandler<Assembly::OpCode::inc>::handle(state, instruction);
                return;
            case Assembly::OpCode::dec:
                InstructionHandler<Assembly::OpCode::dec>::handle(state, instruction);
                return;
            case Assembly::OpCode::mas:
                InstructionHandler<Assembly::OpCode::mas>::handle(state, instruction);
                return;
            case Assembly::OpCode::das:
                InstructionHandler<Assembly::OpCode::das>::handle(state, instruction);
                return;
            case Assembly::OpCode::pas:
                InstructionHandler<Assembly::OpCode::pas>::handle(state, instruction);
                return;
            case Assembly::OpCode::clt:
                InstructionHandler<Assembly::OpCode::clt>::handle(state, instruction);
                return;
            case Assembly::OpCode::cgt:
                InstructionHandler<Assembly::OpCode::cgt>::handle(state, instruction);
                return;
            case Assembly::OpCode::ceq:
                InstructionHandler<Assembly::OpCode::ceq>::handle(state, instruction);
                return;
            case Assembly::OpCode::cne:
                InstructionHandler<Assembly::OpCode::cne>::handle(state, instruction);
                return;
            case Assembly::OpCode::jpz:
                InstructionHandler<Assembly::OpCode::jpz>::handle(state, instruction);
                return;
            case Assembly::OpCode::jmp:
                InstructionHandler<Assembly::OpCode::jmp>::handle(state, instruction);
                return;
            case Assembly::OpCode::hlt:
                InstructionHandler<Assembly::OpCode::hlt>::handle(state, instruction);
                return;

            case Assembly::OpCode::num_op_codes:
                break;
        }
        set_error_state(state, VMErrorCode::unknow_op_code);
    }

    namespace details {
        [[nodiscard]] inline bool
        instruction_indecies_in_range(const Assembly::Instruction& instruction, std::size_t size) noexcept
        {
            return std::cmp_less(instruction.data1(), size) && std::cmp_less(instruction.data2(), size);
        }

        template <std::floating_point T>
        [[nodiscard]] bool instruction_access_in_range(const Assembly::Instruction& instruction, const VMState<T>& state)
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

        [[nodiscard]] inline bool has_fp_exception() noexcept
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

        [[maybe_unused]] inline std::string_view get_error_description() noexcept
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

        template <std::floating_point T>
        void dump_state_fp_error([[maybe_unused]] const VMData& data, [[maybe_unused]] const VMState<T>& state) noexcept
        {
#if RAYCHELSCRIPT_VM_ENABLE_FP_EXCEPTION_DUMP
            Logger::error("Floating-point error during execution: ", get_error_description(), "! Dumping state...\n");
            dump_state(state, data);
#endif
        }
    } // namespace details

    template <std::floating_point T>
    using VMResult = std::variant<VMErrorCode, VMState<T>>;

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
            get_location(state, index.value()) = static_cast<T>(value);
        }

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

#if RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        const auto end = std::chrono::high_resolution_clock::now();

        Logger::info(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(), "ns\n");
#endif

        if (state.error_code != VMErrorCode::ok) {
            return state.error_code;
        }

        return state;
    }

} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_EXECUTE_H
