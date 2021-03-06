/**
* \file Assembler.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for Assembler class
* \date 2021-12-21
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

#include "Assembler/Assembler.h"

#include "Assembler/AssemblingContext.h"
#include "shared/Misc/WalkAST.h"

#include "RaychelCore/AssertingGet.h"
#include "RaychelCore/ClassMacros.h"

#include <utility>

#define TRY(expression, name)                                                                                                    \
    const auto maybe_##name = (expression);                                                                                      \
    if (const auto* ec = std::get_if<AssemblerErrorCode>(&maybe_##name); ec) {                                                   \
        return *ec;                                                                                                              \
    }                                                                                                                            \
    [[maybe_unused]] const auto name = Raychel::get<Assembly::MemoryIndex>(maybe_##name) //NOLINT(bugprone-macro-parentheses)

namespace RaychelScript::Assembler {

    class ScopePusher
    {
    public:
        explicit ScopePusher(AssemblingContext& ctx) : ctx_{ctx}
        {
            ctx.push_scope();
        }

        RAYCHEL_MAKE_NONCOPY_NONMOVE(ScopePusher)

        ~ScopePusher() noexcept
        {
            ctx_.pop_scope();
        }

    private:
        AssemblingContext& ctx_;
    };

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const AST_Node& node, AssemblingContext& ctx) noexcept;

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const AssignmentExpressionData& data, AssemblingContext& ctx) noexcept
    {
        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        ctx.emit<Assembly::OpCode::mov>(rhs_index, lhs_index);
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const ArithmeticExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using Op = ArithmeticExpressionData::Operation;
        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        switch (data.operation) {
            case Op::add:
                ctx.emit<Assembly::OpCode::add>(lhs_index, rhs_index);
                break;
            case Op::subtract:
                ctx.emit<Assembly::OpCode::sub>(lhs_index, rhs_index);
                break;
            case Op::multiply:
                ctx.emit<Assembly::OpCode::mul>(lhs_index, rhs_index);
                break;
            case Op::divide:
                ctx.emit<Assembly::OpCode::div>(lhs_index, rhs_index);
                break;
            case Op::power:
                ctx.emit<Assembly::OpCode::pow>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        const auto result_index = ctx.allocate_intermediate();
        ctx.emit<Assembly::OpCode::mov>(ctx.result_index(), result_index);

        return result_index;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const UpdateExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using Op = UpdateExpressionData::Operation;

        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        switch (data.operation) {
            case Op::add:
                ctx.emit<Assembly::OpCode::inc>(lhs_index, rhs_index);
                break;
            case Op::subtract:
                ctx.emit<Assembly::OpCode::dec>(lhs_index, rhs_index);
                break;
            case Op::multiply:
                ctx.emit<Assembly::OpCode::mas>(lhs_index, rhs_index);
                break;
            case Op::divide:
                ctx.emit<Assembly::OpCode::das>(lhs_index, rhs_index);
                break;
            case Op::power:
                ctx.emit<Assembly::OpCode::pas>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const VariableDeclarationData& data, AssemblingContext& ctx) noexcept
    {
        if (ctx.has_name(data.name)) {
            return AssemblerErrorCode::duplicate_name;
        }
        return ctx.allocate_variable(data.name);
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const VariableReferenceData& data, AssemblingContext& ctx) noexcept
    {
        if (!ctx.has_name(data.name)) {
            return AssemblerErrorCode::unresolved_identifier;
        }
        return ctx.memory_index_for(data.name);
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const NumericConstantData& data, AssemblingContext& ctx) noexcept
    {
        return ctx.allocate_immediate(static_cast<double>(data.value));
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const UnaryExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using Op = UnaryExpressionData::Operation;

        TRY(assemble(data.value_node, ctx), value_index);

        switch (data.operation) {
            case Op::plus:
                break;
            case Op::minus:
            {
                const auto invert_index = ctx.allocate_immediate(-1);
                ctx.emit<Assembly::OpCode::mul>(value_index, invert_index);
                break;
            }
            case Op::factorial:
                ctx.emit<Assembly::OpCode::fac>(value_index);
                break;
            case Op::magnitude:
                ctx.emit<Assembly::OpCode::mag>(value_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        auto intermediate_idx = ctx.allocate_intermediate();
        ctx.emit<Assembly::OpCode::mov>(ctx.result_index(), intermediate_idx);

        return intermediate_idx;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const ConditionalConstructData& data, AssemblingContext& ctx) noexcept
    {
        if (data.body.empty()) {
            return AssemblerErrorCode::ok;
        }

        TRY(assemble(data.condition_node, ctx), condition_index);

        auto jump_instr_index = ctx.emit<Assembly::OpCode::jpz>(Assembly::make_memory_index(0));

        ScopePusher pusher{ctx};

        for (const auto& node : data.body) {
            const auto index_or_error = assemble(node, ctx);
            if (const auto* ec = std::get_if<AssemblerErrorCode>(&index_or_error); ec) {
                if (*ec != AssemblerErrorCode::ok) {
                    return *ec;
                }
            }
        }

        ctx.instructions().at(jump_instr_index).data1() = static_cast<std::uint8_t>(ctx.instruction_index() + 1);

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const RelationalOperatorData& data, AssemblingContext& ctx) noexcept
    {
        using Op = RelationalOperatorData::Operation;
        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        switch (data.operation) {
            case Op::equals:
                ctx.emit<Assembly::OpCode::ceq>(lhs_index, rhs_index);
                break;
            case Op::less_than:
                ctx.emit<Assembly::OpCode::clt>(lhs_index, rhs_index);
                break;
            case Op::greater_than:
                ctx.emit<Assembly::OpCode::cgt>(lhs_index, rhs_index);
                break;
            case Op::not_equals:
                ctx.emit<Assembly::OpCode::cne>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        return ctx.result_index(); //this isn't actually used, but we have no other way to do this
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePushData& data, AssemblingContext& ctx) noexcept
    {
        ctx.push_scope();
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePopData& data, AssemblingContext& ctx) noexcept
    {
        if (!ctx.has_scopes()) {
            return AssemblerErrorCode::invalid_scope_pop;
        }
        ctx.pop_scope();
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const LoopData& data, AssemblingContext& ctx) noexcept
    {
        if (data.body.empty()) {
            return AssemblerErrorCode::ok;
        }

        const auto condition_instruction_index = Assembly::make_memory_index(ctx.instruction_index() + 1);
        TRY(assemble(data.condition_node, ctx), condition_index);

        const auto jump_instruction_index = ctx.emit<Assembly::OpCode::jpz>(Assembly::make_memory_index(0));

        ScopePusher pusher{ctx};

        for (const auto& node : data.body) {
            const auto index_or_error = assemble(node, ctx);
            if (const auto* ec = std::get_if<AssemblerErrorCode>(&index_or_error); ec) {
                if (*ec != AssemblerErrorCode::ok) {
                    return *ec;
                }
            }
        }

        ctx.emit<Assembly::OpCode::jmp>(condition_instruction_index); //jump to the first instruction of the condition

        ctx.instructions().at(jump_instruction_index).data1() = static_cast<std::uint8_t>(ctx.instruction_index() + 1);
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const FunctionCallData& data, AssemblingContext& ctx) noexcept
    {
        (void)data;
        (void)ctx;
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const FunctionReturnData& /*data*/, AssemblingContext& /*ctx*/) noexcept
    {
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] static std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const AST_Node& node, AssemblingContext& ctx) noexcept
    {
        std::variant<AssemblerErrorCode, Assembly::MemoryIndex> maybe_result{};

        apply_handler(node, NodeHandlers{[&](const auto& data) { maybe_result = assemble(data, ctx); }});
        return maybe_result;
    }

    static bool is_jump(const Assembly::OpCode& code)
    {
        return code == Assembly::OpCode::jmp || code == Assembly::OpCode::jpz;
    }

    static void update_jumps(std::vector<Assembly::Instruction>& instructions, auto removed_it) noexcept
    {
        const auto removed_index = std::distance(instructions.begin(), removed_it);

        for (auto& instr : instructions) {
            if (!is_jump(instr.op_code())) {
                continue;
            }

            if (std::cmp_greater(instr.data1(), removed_index)) {
                instr.data1()--;
            }
        }
    }

    static auto
    closest_jump(const std::forward_iterator auto instruction_it, std::vector<Assembly::Instruction>& instructions) noexcept
    {
        for (auto it = instruction_it; it != instructions.begin(); it--) {
            if (is_jump(it->op_code())) {
                return it;
            }
        }
        return instructions.end();
    }

    static void optimize_assembly(AssemblingContext& ctx) noexcept
    {
        auto& instructions = ctx.instructions();
        for (auto it = instructions.begin(); it != instructions.end();) {
            if (it->op_code() != Assembly::OpCode::mov) {
                it++;
                continue;
            }

            //remove MOVs from x to x (x = x -> x)
            if (it->data1() == it->data2()) {
                it = instructions.erase(it);
                update_jumps(instructions, it);
                continue;
            }

            //coalesce MOVs where the first ones destination is the second ones source (y = a; b = y -> b = a)
            if (it != std::prev(instructions.end()) && std::next(it)->op_code() == Assembly::OpCode::mov) {
                const auto next = std::next(it);

                //If only one MOV might be skipped by a jump, we cannot coalesce them
                if (closest_jump(it, instructions) != closest_jump(it, instructions)) {
                    it++;
                    continue;
                }

                const auto destination1 = it->data2();
                const auto source2 = next->data1();
                const auto destination2 = next->data2();

                if (!ctx.is_intermediate(destination1)) {
                    it++;
                    continue;
                }

                if (destination1 == source2) {
                    it = std::prev(instructions.erase(next));
                    update_jumps(instructions, std::next(it));
                    it->data2() = destination2;
                    continue;
                }
            }

            it++;
        }
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, VM::VMData> assemble(const AST& ast) noexcept
    {
        VM::VMData output{};
        AssemblingContext ctx{output.instructions, output.immediate_values};

        for (const auto& identifier : ast.config_block.input_identifiers) {
            output.config_block.input_identifiers.emplace_back(identifier, ctx.allocate_variable(identifier));
        }

        for (const auto& identifier : ast.config_block.output_identifiers) {
            output.config_block.output_identifiers.emplace_back(identifier, ctx.allocate_variable(identifier));
        }

        for (const auto& node : ast.nodes) {
            //TODO: free the intermediates more regularly (maybe whenever we MOV out of one)
            ctx.free_intermediates();
            const auto memory_index_or_error = assemble(node, ctx);
            if (const auto* ec = std::get_if<AssemblerErrorCode>(&memory_index_or_error); ec) {
                if (*ec != AssemblerErrorCode::ok) {
                    return *ec;
                }
                //We drop the memory index here because we have nowhere to put it.
                //This has the effect of removing anything that depends on other instructions to modify state
            }
        }

        ctx.emit<Assembly::OpCode::hlt>(); //the last instruction must be the HLT instruction

        optimize_assembly(ctx);

        output.num_memory_locations = ctx.number_of_memory_locations();

        return output;
    }

} //namespace RaychelScript::Assembler
