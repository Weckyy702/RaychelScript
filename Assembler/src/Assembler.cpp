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
#include "RaychelCore/AssertingGet.h"
#include "shared/Misc/WalkAST.h"

#define TRY(expression, name)                                                                                                    \
    const auto maybe_##name = (expression);                                                                                      \
    if (const auto* ec = std::get_if<AssemblerErrorCode>(&maybe_##name); ec) {                                                   \
        return *ec;                                                                                                              \
    }                                                                                                                            \
    [[maybe_unused]] const auto name = Raychel::get<Assembly::MemoryIndex>(maybe_##name) //NOLINT(bugprone-macro-parentheses)

namespace RaychelScript::Assembler {

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const AST_Node& node, AssemblingContext& ctx) noexcept;

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const AssignmentExpressionData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        ctx.emit<Assembly::OpCode::mov>(rhs_index, lhs_index);
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const ArithmeticExpressionData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        using Op = ArithmeticExpressionData::Operation;
        TRY(assemble(data.lhs, ctx), lhs_index);
        TRY(assemble(data.rhs, ctx), rhs_index);

        ctx.emit<Assembly::OpCode::mov>(lhs_index, ctx.a_index());
        ctx.emit<Assembly::OpCode::mov>(rhs_index, ctx.b_index());

        switch (data.operation) {
            case Op::add:
                ctx.emit<Assembly::OpCode::add>();
                break;
            case Op::subtract:
                ctx.emit<Assembly::OpCode::sub>();
                break;
            case Op::multiply:
                ctx.emit<Assembly::OpCode::mul>();
                break;
            case Op::divide:
                ctx.emit<Assembly::OpCode::div>();
                break;
            case Op::power:
                ctx.emit<Assembly::OpCode::pow>();
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        return ctx.a_index();
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const VariableDeclarationData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        if (ctx.has_name(data.name)) {
            return AssemblerErrorCode::duplicate_name;
        }
        return ctx.allocate_variable(data.name);
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const VariableReferenceData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        if (!ctx.has_name(data.name)) {
            return AssemblerErrorCode::unresolved_identifier;
        }
        return ctx.memory_index_for(data.name);
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const NumericConstantData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        return ctx.allocate_immediate(static_cast<double>(data.value));
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const UnaryExpressionData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        using Op = UnaryExpressionData::Operation;

        TRY(assemble(data.value_node, ctx), value_index);
        ctx.emit<Assembly::OpCode::mov>(value_index, ctx.a_index());

        switch (data.operation) {
            case Op::plus:
                break;
            case Op::minus:
            {
                const auto invert_index = ctx.allocate_immediate(-1);
                ctx.emit<Assembly::OpCode::mov>(invert_index, ctx.b_index());
                ctx.emit<Assembly::OpCode::mul>();
                break;
            }
            case Op::factorial:
                ctx.emit<Assembly::OpCode::fac>();
                break;
            case Op::magnitude:
                ctx.emit<Assembly::OpCode::mag>();
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_expression;
        }

        return ctx.a_index();
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const ConditionalConstructData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        if (data.body.empty()) {
            return AssemblerErrorCode::ok;
        }
        TRY(assemble(data.condition_node, ctx), condition_index);

        auto& jump_instr = ctx.emit<Assembly::OpCode::jpz>(Assembly::make_memory_index(0));

        for (const auto& node : data.body) {
            const auto index_or_error = assemble(node, ctx);
            if (const auto* ec = std::get_if<AssemblerErrorCode>(&index_or_error); ec) {
                if (*ec != AssemblerErrorCode::ok) {
                    return *ec;
                }
            }
        }

        jump_instr.data1() = static_cast<std::uint8_t>(ctx.instruction_index() + 1);

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const LiteralTrueData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        const auto true_index = ctx.allocate_immediate(1);

        ctx.emit<Assembly::OpCode::mov>(true_index, ctx.a_index());
        return ctx.a_index();
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const LiteralFalseData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        const auto false_index = ctx.allocate_immediate(0);

        ctx.emit<Assembly::OpCode::mov>(false_index, ctx.a_index());
        return ctx.a_index();
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const RelationalOperatorData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePushData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePopData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble([[maybe_unused]] const LoopData& data, [[maybe_unused]] AssemblingContext& ctx) noexcept
    {
        return AssemblerErrorCode::not_implemented;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::MemoryIndex>
    assemble(const AST_Node& node, AssemblingContext& ctx) noexcept
    {
        std::variant<AssemblerErrorCode, Assembly::MemoryIndex> maybe_result{};

        apply_handler(node, NodeHandlers{[&](const auto& data) {
                          auto memory_index_or_error = assemble(data, ctx);
                          if (const auto* ec = std::get_if<AssemblerErrorCode>(&memory_index_or_error); ec) {
                              maybe_result = *ec;
                              return;
                          }

                          maybe_result = Raychel::get<Assembly::MemoryIndex>(std::move(memory_index_or_error));
                      }});
        return maybe_result;
    }

    [[nodiscard]] std::variant<AssemblerErrorCode, Assembly::VMData> assemble(const AST& ast) noexcept
    {
        Assembly::VMData output;
        AssemblingContext ctx{output.instructions, output.immediate_values};

        for (const auto& identifier : ast.config_block.input_identifiers) {
            (void)ctx.allocate_variable(identifier);
        }

        for (const auto& identifier : ast.config_block.output_identifiers) {
            (void)ctx.allocate_variable(identifier);
        }

        for (const auto& node : ast.nodes) {
            const auto memory_index_or_error = assemble(node, ctx);
            if (const auto* ec = std::get_if<AssemblerErrorCode>(&memory_index_or_error); ec) {
                if (*ec != AssemblerErrorCode::ok) {
                    return *ec;
                }
                //We drop the memory index here because we have nowhere to put it.
                //This has the effect of removing anything that depends on other instructions to modify state
            }
        }

        ctx.emit<Assembly::OpCode::hlt>();

        for(const auto&[identifier, address] : ctx.names()) {
            Logger::debug('$', address, " -> ", identifier, '\n');
        }

        return output;
    }

} //namespace RaychelScript::Assembler
