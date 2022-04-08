/**
* \file NativeAssembler.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for NativeAssembler class
* \date 2022-04-01
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

#include "NativeAssembler/NativeAssembler.h"
#include <cstring>
#include <iomanip>
#include <set>

#define RAYCHELSCRIPT_NATIVE_ASSEMBLER_DEFINE_ASSEMBLER_FUNCTION(_tag)                                                           \
    NativeAssemblerErrorCode assemble(const VM::VMData& data, _tag##_Tag tag, std::ostream& output_stream) noexcept              \
    {                                                                                                                            \
        if (!output_stream) {                                                                                                    \
            return NativeAssemblerErrorCode::stream_write_error;                                                                 \
        }                                                                                                                        \
        NativeAssemblerState state{output_stream};                                                                               \
        if (const auto ec = write_boilerplate_begin(tag, data, state); ec != NativeAssemblerErrorCode::ok) {                     \
            return ec;                                                                                                           \
        }                                                                                                                        \
        /*We have to do this pass to ensure that every JMP will always have a valid jump target*/                                \
        for (const auto instr : data.instructions) {                                                                             \
            if (instr.op_code() == Assembly::OpCode::jmp || instr.op_code() == Assembly::OpCode::jpz)                            \
                state.jump_indecies.emplace(instr.data1());                                                                      \
        }                                                                                                                        \
                                                                                                                                 \
        for (; state.instruction_index != data.instructions.size(); ++state.instruction_index) {                                 \
            const auto instruction = data.instructions.at(state.instruction_index);                                              \
            TRY(assemble_instruction(tag, instruction, state));                                                                  \
        }                                                                                                                        \
        if (const auto ec = write_boilerplate_end(tag, data, state); ec != NativeAssemblerErrorCode::ok) {                       \
            return ec;                                                                                                           \
        }                                                                                                                        \
        return NativeAssemblerErrorCode::ok;                                                                                     \
    }

#ifdef TRY_WRITE
    #undef TRY_WRITE
#endif
#define TRY_WRITE(exp)                                                                                                           \
    if (!(state.output_stream << exp << '\n')) {                                                                                 \
        return NativeAssemblerErrorCode::stream_write_error;                                                                     \
    }
#ifdef TRY
    #undef TRY
#endif
#define TRY(exp)                                                                                                                 \
    if (const auto ec = (exp); ec != NativeAssemblerErrorCode::ok) {                                                             \
        return ec;                                                                                                               \
    }

namespace RaychelScript::NativeAssembler {

    struct NativeAssemblerState
    {
        std::ostream& output_stream;
        std::size_t instruction_index{0};
        std::set<std::size_t> jump_indecies{};
    };

    inline namespace X86_64 {

        [[nodiscard]] static std::uint64_t get_double_bit_representation(double value) noexcept
        {
            static_assert(sizeof(std::uint64_t) == sizeof(double));
            std::uint64_t bits{};
            std::memcpy(&bits, &value, sizeof(double));
            return bits;
        }

        [[nodiscard]] static std::string memory_index_to_native(X86_64_Tag /*unused*/, std::uint8_t memory_index) noexcept
        {
            std::stringstream ss;
            ss << "qword [rsp+" << static_cast<std::uint32_t>(memory_index - 1) * 8 << ']';
            return ss.str();
        }

        [[nodiscard]] static auto
        write_boilerplate_begin(X86_64_Tag tag, const VM::VMData& data, NativeAssemblerState& state) noexcept
        {
            TRY_WRITE(R"_asm_(
section .text

global raychelscript_entry
global raychelscript_input_vector_size
global raychelscript_output_vector_size

extern pow

raychelscript_entry:
    push r12)_asm_");
            TRY_WRITE("sub rsp, " << static_cast<std::uint32_t>(data.num_memory_locations - 1) * 8);
            TRY_WRITE(R"_asm_(
    mov r12, rdi
    mov rdi, rsp
    xor rax, rax
    mov rcx, )_asm_" << static_cast<std::uint32_t>(data.num_memory_locations-1));
        TRY_WRITE(R"_asm_(
    rep stosq
    mov rdi, r12
                )_asm_");

            for (const auto& [_, address] : data.config_block.input_identifiers) {
                TRY_WRITE("mov rax, qword[rdi+" << static_cast<std::uint32_t>(address.value() - 1) * 8 << ']');
                TRY_WRITE("mov " << memory_index_to_native(tag, address.value()) << ", rax");
            }
            for (const auto& [value, address] : data.immediate_values) {
                TRY_WRITE("mov rax, " << get_double_bit_representation(value) << "; = " << value);
                TRY_WRITE("mov " << memory_index_to_native(tag, address.value()) << ", rax");
            }
            TRY_WRITE(R"_asm_(
    xor rax, rax
    mov r12, 1
                )_asm_");
            return NativeAssemblerErrorCode::ok;
        }

        [[nodiscard]] static NativeAssemblerErrorCode
        assemble_instruction(X86_64_Tag tag, const Assembly::Instruction& instruction, NativeAssemblerState& state)
        {
            using Op = Assembly::OpCode;

            if (state.jump_indecies.contains(state.instruction_index)) {
                TRY_WRITE("label" << state.instruction_index << ':');
                state.jump_indecies.erase(state.instruction_index);
            }

            switch (instruction.op_code()) {
                case Op::mov:
                    if (instruction.data1() == 0) {
                        TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data2()) << ", xmm0");
                    } else {
                        TRY_WRITE("mov rax, " << memory_index_to_native(tag, instruction.data1()))
                        TRY_WRITE("mov " << memory_index_to_native(tag, instruction.data2()) << ", rax");
                    }
                    return NativeAssemblerErrorCode::ok;
                case Op::add:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("addsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    return NativeAssemblerErrorCode::ok;
                case Op::sub:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("subsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    return NativeAssemblerErrorCode::ok;
                case Op::mul:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("mulsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    return NativeAssemblerErrorCode::ok;
                case Op::div:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("divsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    return NativeAssemblerErrorCode::ok;
                case Op::mag:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("movsd xmm1, qword [rel raychelscript_mag_op]");
                    TRY_WRITE("pand xmm0, xmm1");
                    return NativeAssemblerErrorCode::ok;
                case Op::fac:
                    return NativeAssemblerErrorCode::unknown_instruction;
                case Op::pow:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("movsd xmm1, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("push rsi");
                    TRY_WRITE("call pow wrt ..plt");
                    TRY_WRITE("pop rsi");
                    return NativeAssemblerErrorCode::ok;
                case Op::inc:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("addsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data1()) << ", xmm0");
                    return NativeAssemblerErrorCode::ok;
                case Op::dec:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("subsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data1()) << ", xmm0");
                    return NativeAssemblerErrorCode::ok;
                case Op::mas:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("mulsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data1()) << ", xmm0");
                    return NativeAssemblerErrorCode::ok;
                case Op::das:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("divsd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data1()) << ", xmm0");
                    return NativeAssemblerErrorCode::ok;
                case Op::pas:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("movsd xmm1, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("push rsi");
                    TRY_WRITE("call pow wrt ..plt");
                    TRY_WRITE("pop rsi");
                    TRY_WRITE("movsd " << memory_index_to_native(tag, instruction.data1()) << ", xmm0");
                    return NativeAssemblerErrorCode::ok;
                case Op::clt:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("comisd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("cmovb rax, r12");
                    return NativeAssemblerErrorCode::ok;
                case Op::cgt:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("comisd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("cmova rax, r12");
                    return NativeAssemblerErrorCode::ok;
                case Op::ceq:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("comisd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("cmove rax, r12");
                    return NativeAssemblerErrorCode::ok;
                case Op::cne:
                    TRY_WRITE("movsd xmm0, " << memory_index_to_native(tag, instruction.data1()));
                    TRY_WRITE("comisd xmm0, " << memory_index_to_native(tag, instruction.data2()));
                    TRY_WRITE("cmovne rax, r12");
                    return NativeAssemblerErrorCode::ok;
                case Op::jpz:
                    TRY_WRITE("test rax, rax");
                    TRY_WRITE("jz label" << static_cast<std::uint32_t>(instruction.data1()));
                    TRY_WRITE("xor rax, rax");
                    return NativeAssemblerErrorCode::ok;
                case Op::jmp:
                    TRY_WRITE("jmp label" << static_cast<std::uint32_t>(instruction.data1()));
                    return NativeAssemblerErrorCode::ok;
                case Op::hlt:
                case Op::num_op_codes:
                    return NativeAssemblerErrorCode::ok;
            }
            return NativeAssemblerErrorCode::unknown_instruction;
        }

        static auto write_boilerplate_end(X86_64_Tag tag, const VM::VMData& data, NativeAssemblerState& state) noexcept
        {
            std::size_t i{0};
            for (const auto& [_, address] : data.config_block.output_identifiers) {
                TRY_WRITE("mov rax, " << memory_index_to_native(tag, address.value()));
                TRY_WRITE("mov qword[rsi+" << i << "], rax");
                i += 8;
            }
            TRY_WRITE("add rsp, " << static_cast<std::uint32_t>(data.num_memory_locations - 1) * 8);
            TRY_WRITE("pop r12");
            TRY_WRITE(R"_asm_(    ret
section .rodata)_asm_");
            TRY_WRITE(
                "raychelscript_input_vector_size: dd " << static_cast<std::uint32_t>(data.config_block.input_identifiers.size()));
            TRY_WRITE(
                "raychelscript_output_vector_size: dd "
                << static_cast<std::uint32_t>(data.config_block.output_identifiers.size()));
            TRY_WRITE("raychelscript_mag_op: dq 0x7FFFFFFFFFFFFFFF");

            return NativeAssemblerErrorCode::ok;
        }
    } // namespace X86_64

    RAYCHELSCRIPT_NATIVE_ASSEMBLER_DEFINE_ASSEMBLER_FUNCTION(X86_64)

} //namespace RaychelScript::NativeAssembler
