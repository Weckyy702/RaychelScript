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
#include "RaychelCore/Finally.h"
#include "RaychelCore/ScopedTimer.h"

#define RAYCHELSCRIPT_ASSEMBLER_VERBOSE 1

#define TRY(expression, name)                                                                                                    \
    auto maybe_##name = (expression);                                                                                            \
    if (const auto* ec = std::get_if<AssemblerErrorCode>(&maybe_##name); ec) {                                                   \
        return *ec;                                                                                                              \
    }                                                                                                                            \
    auto name = Raychel::get<Assembly::MemoryIndex>(maybe_##name); //NOLINT(bugprone-macro-parentheses)

#define TRY_NO_INDEX(expression)                                                                                                 \
    {                                                                                                                            \
        const auto maybe_result = (expression);                                                                                  \
        if (const auto* ec = std::get_if<AssemblerErrorCode>(&maybe_result); ec != nullptr && *ec != AssemblerErrorCode::ok)     \
            return *ec;                                                                                                          \
    }

#if RAYCHELSCRIPT_ASSEMBLER_VERBOSE
    #define RAYCHELSCRIPT_ASSEMBLER_DEBUG(...) Logger::debug(ctx.indent(), __VA_ARGS__)
#else
    #define RAYCHELSCRIPT_ASSEMBLER_DEBUG(...)
#endif

namespace RaychelScript::Assembler {

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const AST_Node& node, AssemblingContext& ctx) noexcept;

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const AssignmentExpressionData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling assignment expression\n");

        TRY(assemble(data.rhs, ctx), rhs_index);
        TRY(assemble(data.lhs, ctx), lhs_index);

        if (rhs_index != lhs_index) {
            ctx.emit<OpCode::mov>(rhs_index, lhs_index);
        }

        return AssemblerErrorCode::ok;
    }

    [[maybe_unused]] static char op_to_string(ArithmeticExpressionData::Operation op)
    {
        using enum ArithmeticExpressionData::Operation;

        switch (op) {
            case add:
                return '+';
            case subtract:
                return '-';
            case multiply:
                return '*';
            case divide:
                return '/';
            case power:
                return '^';
        }
        return '?';
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const ArithmeticExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using enum ArithmeticExpressionData::Operation;

        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling arithmetic operator ", op_to_string(data.operation), '\n');

        TRY(assemble(data.rhs, ctx), rhs_index);

        //RHS-result must be saved in an intermediate because the A register is volatile and can be overriden by LHS
        if (rhs_index == ctx.a_index()) {
            RAYCHELSCRIPT_ASSEMBLER_DEBUG("Saving result of right-hand-side subexpression in intermediate\n");

            const auto intermediate = ctx.allocate_intermediate();
            ctx.emit<OpCode::mov>(rhs_index, intermediate);
            rhs_index = intermediate;
        }

        TRY(assemble(data.lhs, ctx), lhs_index)

        switch (data.operation) {
            case add:
                ctx.emit<OpCode::add>(lhs_index, rhs_index);
                break;
            case subtract:
                ctx.emit<OpCode::sub>(lhs_index, rhs_index);
                break;
            case multiply:
                ctx.emit<OpCode::mul>(lhs_index, rhs_index);
                break;
            case divide:
                ctx.emit<OpCode::div>(lhs_index, rhs_index);
                break;
            case power:
                ctx.emit<OpCode::pow>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_operation;
        }

        ctx.free_intermediate(rhs_index);
        return ctx.a_index();
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const UpdateExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using enum ArithmeticExpressionData::Operation;
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling update expression ", op_to_string(data.operation), "=\n");

        TRY(assemble(data.rhs, ctx), rhs_index)
        TRY(assemble(data.lhs, ctx), lhs_index)

        switch (data.operation) {
            case add:
                ctx.emit<OpCode::inc>(lhs_index, rhs_index);
                break;
            case subtract:
                ctx.emit<OpCode::dec>(lhs_index, rhs_index);
                break;
            case multiply:
                ctx.emit<OpCode::mas>(lhs_index, rhs_index);
                break;
            case divide:
                ctx.emit<OpCode::das>(lhs_index, rhs_index);
                break;
            case power:
                ctx.emit<OpCode::pas>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_operation;
        }

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const VariableDeclarationData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling variable declaration '", data.name, "'\n");
        return ctx.add_variable(data.name);
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const VariableReferenceData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling variable reference '", data.name, "'\n");
        return ctx.index_for(data.name);
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const NumericConstantData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling numeric constant ", data.value, '\n');
        return ctx.allocate_immediate(static_cast<double>(data.value));
    }

    [[maybe_unused]] static char op_to_string(UnaryExpressionData::Operation op)
    {
        using enum UnaryExpressionData::Operation;

        switch (op) {
            case minus:
                return '-';
            case plus:
                return '+';
            case factorial:
                return '!';
            case magnitude:
                return '|';
        }
        return '\0';
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const UnaryExpressionData& data, AssemblingContext& ctx) noexcept
    {
        using enum UnaryExpressionData::Operation;

        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling unary expression ", op_to_string(data.operation), '\n');

        TRY(assemble(data.value_node, ctx), value_index)

        switch (data.operation) {
            case minus:
                ctx.emit<OpCode::mul>(value_index, ctx.allocate_immediate(-1.0));
                break;
            case plus:
                return value_index; //nop
            case factorial:
                ctx.emit<OpCode::fac>(value_index);
                break;
            case magnitude:
                ctx.emit<OpCode::mag>(value_index);
                break;
            default:
                return AssemblerErrorCode::unknown_arithmetic_operation;
        }

        return ctx.a_index();
    }

    [[nodiscard]] static MemoryIndex make_jump_offset(std::size_t jump_index, std::size_t destination_index) noexcept
    {
        RAYCHEL_ASSERT(std::in_range<std::ptrdiff_t>(jump_index));
        RAYCHEL_ASSERT(std::in_range<std::ptrdiff_t>(destination_index));

        return make_memory_index(
            static_cast<std::ptrdiff_t>(destination_index) - static_cast<std::ptrdiff_t>(jump_index),
            MemoryIndex::ValueType::jump_offset);
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const ConditionalConstructData& data, AssemblingContext& ctx) noexcept
    {

        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling conditional construct\n");

        TRY_NO_INDEX(assemble(data.condition_node, ctx));

        const auto jpz_instruction_index = ctx.emit<OpCode::jpz>();

        {
            ScopePusher _{ctx, true, "if"};
            for (const auto& node : data.body) {
                TRY_NO_INDEX(assemble(node, ctx));
            }
        }

        if (data.else_body.empty()) {
            ctx.instructions().at(jpz_instruction_index).index1() =
                make_jump_offset(jpz_instruction_index, ctx.next_instruction_index());
            return AssemblerErrorCode::ok;
        }

        const auto jmp_index = ctx.emit<OpCode::jmp>();
        ctx.instructions().at(jpz_instruction_index).index1() =
            make_jump_offset(jpz_instruction_index, ctx.next_instruction_index());

        {
            ScopePusher _{ctx, true, "else"};
            for (const auto& node : data.else_body) {
                TRY_NO_INDEX(assemble(node, ctx));
            }
        }
        ctx.instructions().at(jmp_index).index1() = make_jump_offset(jmp_index, ctx.next_instruction_index());

        return AssemblerErrorCode::ok;
    }

    [[maybe_unused]] static std::string_view op_to_string(RelationalOperatorData::Operation op)
    {
        using enum RelationalOperatorData::Operation;
        using namespace std::string_view_literals;

        switch (op) {
            case equals:
                return "=="sv;
            case not_equals:
                return "!="sv;
            case less_than:
                return "<"sv;
            case greater_than:
                return ">"sv;
        }

        return {};
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble(const RelationalOperatorData& data, AssemblingContext& ctx) noexcept
    {
        using enum RelationalOperatorData::Operation;

        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling relational operator ", op_to_string(data.operation), '\n');

        TRY(assemble(data.rhs, ctx), rhs_index);
        TRY(assemble(data.lhs, ctx), lhs_index);

        switch (data.operation) {
            case equals:
                ctx.emit<OpCode::ceq>(lhs_index, rhs_index);
                break;
            case not_equals:
                ctx.emit<OpCode::cne>(lhs_index, rhs_index);
                break;
            case less_than:
                ctx.emit<OpCode::clt>(lhs_index, rhs_index);
                break;
            case greater_than:
                ctx.emit<OpCode::cgt>(lhs_index, rhs_index);
                break;
            default:
                return AssemblerErrorCode::unknown_relational_operation;
        }

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePushData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling inline scope push\n");

        (void)data;
        ctx.push_scope(true, "inline");
        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex>
    assemble([[maybe_unused]] const InlinePopData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling inline scope pop\n");

        (void)data;
        return ctx.pop_scope("inline");
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const LoopData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling loop\n");

        const auto condition_index = ctx.next_instruction_index();
        TRY_NO_INDEX(assemble(data.condition_node, ctx));

        const auto jpz_instruction_index = ctx.emit<OpCode::jpz>();

        {
            ScopePusher _{ctx, true, "loop"};

            for (const auto& node : data.body) {
                TRY_NO_INDEX(assemble(node, ctx));
            }
        }

        ctx.emit<OpCode::jmp>(make_jump_offset(ctx.next_instruction_index(), condition_index));

        ctx.instructions().at(jpz_instruction_index).index1() =
            make_jump_offset(jpz_instruction_index, ctx.next_instruction_index());

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const FunctionCallData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling function call ", data.mangled_callee_name, '\n');

        //Check if function exists / get its index

        if (const auto ec = ctx.mark_function(data.mangled_callee_name); ec != AssemblerErrorCode::ok)
            return ec;
        TRY(ctx.find_function(data.mangled_callee_name), function_index);

        std::vector<MemoryIndex> argument_indecies{};

        for (auto it = data.argument_expressions.begin(); it != data.argument_expressions.end(); ++it) {
            TRY(assemble(*it, ctx), argument_index);

            //The A index is volatile, so everyone except for the last argument must MOV out of it
            if (argument_index == ctx.a_index() && it != std::prev(data.argument_expressions.end())) {
                auto intermediate_index = ctx.allocate_intermediate();
                ctx.emit<OpCode::mov>(argument_index, intermediate_index);
                argument_indecies.push_back(intermediate_index);
                continue;
            }
            argument_indecies.push_back(argument_index);
        }

        //PUT all function arguments onto the stack
        std::size_t argument_index{1};
        for (const auto& arg : argument_indecies) {
            ctx.emit<OpCode::put>(arg, make_memory_index(argument_index++, MemoryIndex::ValueType::stack));
            ctx.free_intermediate(arg);
        }

        //jump there
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Jumping to subroutine ", function_index, '\n');
        ctx.emit<OpCode::jsr>(function_index);

        return ctx.a_index();
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const FunctionReturnData& data, AssemblingContext& ctx) noexcept
    {
        RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling return expression\n");

        TRY(assemble(data.return_value, ctx), return_index);

        if (return_index != ctx.a_index()) {
            ctx.emit<OpCode::mov>(return_index, ctx.a_index());
        }

        ctx.emit<OpCode::ret>();

        return AssemblerErrorCode::ok;
    }

    [[nodiscard]] static ErrorOr<Assembly::MemoryIndex> assemble(const AST_Node& node, AssemblingContext& ctx) noexcept
    {
        ErrorOr<Assembly::MemoryIndex> maybe_result{};

        ++ctx.debug_depth;
        apply_handler(node, NodeHandlers{[&](const auto& data) { maybe_result = assemble(data, ctx); }});
        --ctx.debug_depth;
        return maybe_result;
    }

    [[nodiscard]] ErrorOr<VM::VMData> assemble(const AST& ast) noexcept
    {
        [[maybe_unused]] Raychel::ScopedTimer<std::chrono::microseconds> timer{"Assembling time"};

        VM::VMData output{};
        AssemblingContext ctx{ast, output};

        for (const auto& name : ast.config_block.input_identifiers) {
            TRY(ctx.add_variable(name), index)
            ++output.num_input_identifiers;
            RAYCHELSCRIPT_ASSEMBLER_DEBUG("Added input constant '", name, "' with index ", index, '\n');
        }

        for (const auto& name : ast.config_block.output_identifiers) {
            TRY(ctx.add_variable(name), index)
            ++output.num_output_identifiers;
            RAYCHELSCRIPT_ASSEMBLER_DEBUG("Added output variable '", name, "' with index ", index, '\n');
        }

        for (const auto& node : ast.nodes) {
            TRY_NO_INDEX(assemble(node, ctx));
        }

        ctx.emit<OpCode::hlt>();

        while (ctx.has_marked_functions()) {
            const auto function = ctx.next_marked_function();
            RAYCHELSCRIPT_ASSEMBLER_DEBUG("Assembling function '", function.mangled_name, "'\n");
            ctx.push_function_scope(function.mangled_name);
            for (const auto& arg : function.arguments) {
                TRY(ctx.add_variable(arg), index);
                (void)index;
                RAYCHELSCRIPT_ASSEMBLER_DEBUG("Added argument '", arg, "' to index ", index, '\n');
            }
            for (const auto& node : function.body) {
                TRY_NO_INDEX(assemble(node, ctx));
            }
            ctx.pop_function_scope(function.mangled_name);
        }

        return output;
    }

} //namespace RaychelScript::Assembler
