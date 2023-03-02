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
#include "VM/VMErrorCode.h"
#define RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING 1
#define RAYCHELSCRIPT_VM_SILENT 1
#define RAYCHELSCRIPT_VM_EXECUTION_TYPE RAYCHELSCRIPT_VM_EXECUTION_TYPE_COMPUTED_GOTO

#include "VM/VM.h"

#include "RaychelCore/ScopedTimer.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <utility>

#pragma STDC FENV_ACCESS ON

#if !RAYCHELSCRIPT_VM_SILENT
    #define RAYCHELSCRIPT_VM_DEBUG(...) Logger::debug(indent(state), __VA_ARGS__, '\n')
#else
    #define RAYCHELSCRIPT_VM_DEBUG(...)
#endif

#define RAYCHELSCRIPT_VM_THROW(_error)                                                                                           \
    {                                                                                                                            \
        state.error = _error;                                                                                                    \
        state.halt_flag = true;                                                                                                  \
        return;                                                                                                                  \
    }

#define RAYCHELSCRIPT_VM_EXECUTION_TYPE_SWITCH 1
#define RAYCHELSCRIPT_VM_EXECUTION_TYPE_COMPUTED_GOTO 2

#define RAYCHELSCRIPT_VM_EXECUTION_TYPE_DEFAULT RAYCHELSCRIPT_VM_EXECUTION_TYPE_SWITCH

namespace RaychelScript::VM {

    using Assembly::MemoryIndex;

    static double& get_location(VMState& state, std::uint8_t index) noexcept
    {
        return *(state.stack_pointer + static_cast<std::ptrdiff_t>(index));
    }

    static double& get_location(VMState& state, MemoryIndex index) noexcept
    {
        return get_location(state, index.value());
    }

    static double& result_location(VMState& state) noexcept
    {
        return *state.stack_pointer;
    }

    static double get_value(VMState& state, MemoryIndex index) noexcept
    {
        if (index.type() == MemoryIndex::ValueType::immediate)
            return state.data.immediate_values[index.value()];
        return get_location(state, index);
    }

    void push_frame(VMState& state, const CallFrameDescriptor& descriptor) noexcept
    {
        if (state.frame_pointer == std::prev(state.end_of_stack)) [[unlikely]]
            RAYCHELSCRIPT_VM_THROW(VMErrorCode::stack_overflow);

        new (std::to_address(++state.frame_pointer))
            VMState::CallFrame{descriptor.instructions.begin(), static_cast<std::ptrdiff_t>(descriptor.size)};
    }

    static void update_instruction_pointer(VMState& state, MemoryIndex offset)
    {
        state.frame_pointer->instruction_pointer += static_cast<std::ptrdiff_t>(static_cast<std::int8_t>(offset.value()) - 1);
    }

    [[maybe_unused]] static auto indent(VMState& state)
    {
        return std::string_view(
            "|..................................................................................................................."
            ".....................................................................................",
            state.call_depth * 2 + 1);
    }

    // Instruction Handlers
    //NOLINTBEGIN(bugprone-easily-swappable-parameters)

    static void handle_mov(VMState& state, MemoryIndex from, MemoryIndex to) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_mov: ", from, " (", get_value(state, from), ") -> ", to);
        get_location(state, to) = get_value(state, from);
    }

    static void handle_add(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_add: ", a, " (", get_value(state, a), ") + ", b, " (", get_value(state, b), ')');

        result_location(state) = get_value(state, a) + get_value(state, b);
    }

    static void handle_sub(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_sub: ", a, " (", get_value(state, a), ") - ", b, " (", get_value(state, b), ')');

        result_location(state) = get_value(state, a) - get_value(state, b);
    }

    static void handle_mul(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_mul: ", a, " (", get_value(state, a), ") * ", b, " (", get_value(state, b), ')');

        result_location(state) = get_value(state, a) * get_value(state, b);
    }

    static void handle_div(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_div: ", a, " (", get_value(state, a), ") / ", b, " (", get_value(state, b), ')');

        const auto divisor = get_value(state, b);

        if (divisor == 0.0) [[unlikely]]
            RAYCHELSCRIPT_VM_THROW(VMErrorCode::divide_by_zero);

        result_location(state) = get_value(state, a) / divisor;
    }

    static void handle_mag(VMState& state, MemoryIndex a) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_mag: ", a, " (", get_value(state, a), ')');

        result_location(state) = std::abs(get_value(state, a));
    }

    static void handle_fac(VMState& state, MemoryIndex a) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_fac: ", a, " (", get_value(state, a), ')');

        result_location(state) = std::tgamma(get_value(state, a) + 1);

        if constexpr (math_errhandling & MATH_ERREXCEPT) { // NOLINT(hicpp-signed-bitwise)
            if (std::fetestexcept(FE_DIVBYZERO) != 0) [[unlikely]]
                RAYCHELSCRIPT_VM_THROW(VMErrorCode::divide_by_zero);
            if (std::fetestexcept(FE_INVALID) != 0) [[unlikely]]
                RAYCHELSCRIPT_VM_THROW(VMErrorCode::invalid_operand);
        } else {
            const auto _errno = errno;
            if (_errno == EDOM) [[unlikely]]
                RAYCHELSCRIPT_VM_THROW(VMErrorCode::invalid_operand);
            if (_errno == ERANGE) [[unlikely]]
                RAYCHELSCRIPT_VM_THROW(VMErrorCode::divide_by_zero);
        }
    }

    static void handle_pow(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_pow: ", a, " (", get_value(state, a), ") ^ ", b, " (", get_value(state, b), ')');

        result_location(state) = std::pow(get_value(state, a), get_value(state, b));
    }

    static void handle_inc(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_inc: ", a, " (", get_value(state, a), ") += ", b, " (", get_value(state, b), ')');

        get_location(state, a) += get_value(state, b);
    }

    static void handle_dec(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_dec: ", a, " (", get_value(state, a), ") -= ", b, " (", get_value(state, b), ')');

        get_location(state, a) -= get_value(state, b);
    }

    static void handle_mas(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_mas: ", a, " (", get_value(state, a), ") *= ", b, " (", get_value(state, b), ')');

        get_location(state, a) *= get_value(state, b);
    }

    static void handle_das(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_das: ", a, " (", get_value(state, a), ") /= ", b, " (", get_value(state, b), ')');

        const auto divisor = get_value(state, b);

        if (divisor == 0.0) [[unlikely]]
            RAYCHELSCRIPT_VM_THROW(VMErrorCode::divide_by_zero);

        get_location(state, a) /= get_value(state, b);
    }

    static void handle_pas(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_pas: ", a, " (", get_value(state, a), ") ^= ", b, " (", get_value(state, b), ')');

        auto& res = get_location(state, a);

        res = std::pow(res, get_value(state, b));
    }

    static void handle_clt(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_clt: ", a, " (", get_value(state, a), ") < ", b, " (", get_value(state, b), ')');

        state.flag = get_value(state, a) < get_value(state, b);
    }

    static void handle_cgt(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_cgt: ", a, " (", get_value(state, a), ") > ", b, " (", get_value(state, b), ')');

        state.flag = get_value(state, a) > get_value(state, b);
    }

    static void handle_ceq(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_ceq: ", a, " (", get_value(state, a), ") == ", b, " (", get_value(state, b), ')');

        state.flag = get_value(state, a) == get_value(state, b);
    }

    static void handle_cne(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_cne: ", a, " (", get_value(state, a), ") != ", b, " (", get_value(state, b), ')');

        state.flag = get_value(state, a) != get_value(state, b);
    }

    static void handle_jpz(VMState& state, MemoryIndex a) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_jpz: ", a, state.flag ? ": true" : ": false");

        if (state.flag)
            return;
        update_instruction_pointer(state, a);
    }

    static void handle_jmp(VMState& state, MemoryIndex a) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_jmp: ", a);

        update_instruction_pointer(state, a);
    }

    static void handle_jsr(VMState& state, MemoryIndex a) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_jsr: ", a);

        const auto& descriptor = state.data.call_frames[a.value()];

        //It's ok to possibly corrput the stack pointer here because we will immediately bail out if we do
        state.stack_pointer += state.frame_pointer->size;
        if (state.stack_pointer >= state.end_of_memory) [[unlikely]]
            RAYCHELSCRIPT_VM_THROW(VMErrorCode::memory_overflow);

        push_frame(state, descriptor);

        ++state.call_depth;
        ++state.function_call_count;
    }

    static void handle_ret(VMState& state) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_ret");

        if (state.frame_pointer == state.beginning_of_stack) [[unlikely]]
            RAYCHELSCRIPT_VM_THROW(VMErrorCode::stack_underflow);

        //we need to transfer the zero location since it contains the result of the call
        const auto result = result_location(state);
        state.stack_pointer -= std::prev(state.frame_pointer)->size;
        --state.frame_pointer;
        result_location(state) = result;

        --state.call_depth;
    }

    static void handle_put(VMState& state, MemoryIndex a, MemoryIndex b) noexcept
    {
        RAYCHELSCRIPT_VM_DEBUG("handle_put: ", a, " (", get_value(state, a), ") -> ", b);

        const auto next_stack_pointer = state.stack_pointer + state.frame_pointer->size;

        *(next_stack_pointer + static_cast<std::ptrdiff_t>(b.value())) = get_value(state, a);
    }

    //NOLINTEND(bugprone-easily-swappable-parameters)
    // Main execution loop

    VMErrorCode do_execute(VMState& state)
    {
#if RAYCHELSCRIPT_VM_EXECUTION_TYPE == RAYCHELSCRIPT_VM_EXECUTION_TYPE_SWITCH
        while (!state.halt_flag) [[likely]] {
            using enum Assembly::OpCode;

            ++state.instruction_count;
            const auto instruction = *(state.frame_pointer->instruction_pointer++);
            const auto a = instruction.index1();
            const auto b = instruction.index2();

            switch (instruction.op_code()) {
                case mov:
                    handle_mov(state, instruction.index1(), instruction.index2());
                    break;
                case add:
                    handle_add(state, a, b);
                    break;
                case sub:
                    handle_sub(state, a, b);
                    break;
                case mul:
                    handle_mul(state, a, b);
                    break;
                case div:
                    handle_div(state, a, b);
                    break;
                case mag:
                    handle_mag(state, a);
                    break;
                case fac:
                    handle_fac(state, a);
                    break;
                case pow:
                    handle_pow(state, a, b);
                    break;
                case inc:
                    handle_inc(state, a, b);
                    break;
                case dec:
                    handle_dec(state, a, b);
                    break;
                case mas:
                    handle_mas(state, a, b);
                    break;
                case das:
                    handle_das(state, a, b);
                    break;
                case pas:
                    handle_pas(state, a, b);
                    break;
                case clt:
                    handle_clt(state, a, b);
                    break;
                case cgt:
                    handle_cgt(state, a, b);
                    break;
                case ceq:
                    handle_ceq(state, a, b);
                    break;
                case cne:
                    handle_cne(state, a, b);
                    break;
                case jpz:
                    handle_jpz(state, a);
                    break;
                case jmp:
                    handle_jmp(state, a);
                    break;
                case hlt:
                    return state.error;
                case jsr:
                    handle_jsr(state, a);
                    break;
                case ret:
                    handle_ret(state);
                    break;
                case put:
                    handle_put(state, a, b);
                    break;
                default:
                    return VMErrorCode::unknown_opcode;
            }
        }
        return state.error;

#elif RAYCHELSCRIPT_VM_EXECUTION_TYPE == RAYCHELSCRIPT_VM_EXECUTION_TYPE_COMPUTED_GOTO

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"

        constexpr static std::array labels{
            &&done, &&unknown_opcode,

            &&mov,  &&add,
            &&sub,  &&mul,
            &&div,  &&mag,
            &&fac,  &&pow,
            &&inc,  &&dec,
            &&mas,  &&das,
            &&pas,  &&clt,
            &&cgt,  &&ceq,
            &&cne,  &&jpz,
            &&jmp,  &&hlt,
            &&jsr,  &&ret,
            &&put,
        };

        MemoryIndex index1{};
        MemoryIndex index2{};

        const auto next = [&] {
            if (state.halt_flag) [[unlikely]]
                return labels[0]; //done

            ++state.instruction_count;
            const auto& instruction = *(state.frame_pointer->instruction_pointer++);
            const auto code = instruction.op_code();
            if (code >= Assembly::OpCode::num_op_codes) [[unlikely]]
                return labels[1]; //unknown_opcode

            index1 = instruction.index1();
            index2 = instruction.index2();

            //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): we have checked the index before
            return labels[static_cast<std::size_t>(instruction.op_code()) + 2];
        };

        goto* next();

    mov:
        handle_mov(state, index1, index2);
        goto* next();
    add:
        handle_add(state, index1, index2);
        goto* next();
    sub:
        handle_sub(state, index1, index2);
        goto* next();
    mul:
        handle_mul(state, index1, index2);
        goto* next();
    div:
        handle_div(state, index1, index2);
        goto* next();
    mag:
        handle_mag(state, index1);
        goto* next();
    fac:
        handle_fac(state, index1);
        goto* next();
    pow:
        handle_pow(state, index1, index2);
        goto* next();
    inc:
        handle_inc(state, index1, index2);
        goto* next();
    dec:
        handle_dec(state, index1, index2);
        goto* next();
    mas:
        handle_mas(state, index1, index2);
        goto* next();
    das:
        handle_das(state, index1, index2);
        goto* next();
    pas:
        handle_pas(state, index1, index2);
        goto* next();
    clt:
        handle_clt(state, index1, index2);
        goto* next();
    cgt:
        handle_cgt(state, index1, index2);
        goto* next();
    ceq:
        handle_ceq(state, index1, index2);
        goto* next();
    cne:
        handle_cne(state, index1, index2);
        goto* next();
    jpz:
        handle_jpz(state, index1);
        goto* next();
    jmp:
        handle_jmp(state, index1);
        goto* next();
    hlt:
        return VMErrorCode::ok;
    jsr:
        handle_jsr(state, index1);
        goto* next();
    ret:
        handle_ret(state);
        goto* next();
    put:
        handle_put(state, index1, index2);
        goto* next();
    unknown_opcode:
        return VMErrorCode::unknown_opcode;
    done:
        return state.error;
    #pragma GCC diagnostic pop
#else
    #error "Unknown execution type"
#endif
    }

    VMErrorCode execute(
        const VMData& data, std::span<const double> input_variables, std::span<double> output_values, std::size_t stack_size,
        std::size_t memory_size, std::pmr::memory_resource* resource) noexcept
    {
#ifdef RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        const auto start = std::chrono::high_resolution_clock::now();
#endif
        //Bail out early if the input sizes don't match up
        if (std::cmp_not_equal(input_variables.size(), data.num_input_identifiers))
            return VMErrorCode::mismatched_inputs;

        if (std::cmp_not_equal(output_values.size(), data.num_output_identifiers))
            return VMErrorCode::mismatched_outputs;

        //NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto* call_stack = reinterpret_cast<VMState::CallFrame*>(
            resource->allocate(stack_size * sizeof(VMState::CallFrame), alignof(VMState::CallFrame)));

        DynamicArray<double> memory(memory_size, 0.0, resource);

        //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        VMState state{details::Range{memory}, details::Range{call_stack, call_stack + stack_size}, data};

#ifdef RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING
        [[maybe_unused]] Raychel::Finally _{[start, &state] {
            const auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Executed " << state.instruction_count << " instructions (" << state.function_call_count
                      << " function calls) in " << duration_cast<std::chrono::microseconds>(end - start).count() << "µs\n";
        }};
#endif

        std::size_t i{1};
        for (const auto& value : input_variables) {
            RAYCHELSCRIPT_VM_DEBUG("Assigning input value ", value, " to address $", static_cast<std::uint32_t>(i));
            memory[i] = value;
        }

        if (const auto ec = do_execute(state); ec != VMErrorCode::ok) [[unlikely]]
            return ec;

        for (uint8_t j{}; j != data.num_output_identifiers; ++j) {
            const auto value = memory[i + j];
            RAYCHELSCRIPT_VM_DEBUG(
                "storing output variable #",
                static_cast<std::uint32_t>(j),
                " (from $",
                static_cast<std::uint32_t>(i + j),
                "): ",
                value);
            output_values[j] = value;
        }

        return VMErrorCode::ok;
    }
} // namespace RaychelScript::VM
