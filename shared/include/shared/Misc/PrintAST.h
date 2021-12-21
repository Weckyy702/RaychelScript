/**
* \file PrintAST.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ast printing functions
* \date 2021-12-05
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
#ifndef RAYCHELSCRIPT_PRINT_AST_H
#define RAYCHELSCRIPT_PRINT_AST_H

#include "shared/AST/AST.h"
#include "shared/AST/NodeData.h"
#include "shared/IndentHandler.h"

#include "WalkAST.h"

namespace RaychelScript {

    inline void print_node(const RaychelScript::AST_Node& node, std::string_view prefix) noexcept;

    namespace details {

        inline void handle_assignment_data(const RaychelScript::AssignmentExpressionData& data) noexcept
        {
            Logger::log("ASSIGN\n");
            print_node(data.lhs, "lhs=");
            print_node(data.rhs, "rhs=");
        }

        inline void handle_numeric_constant_data(const RaychelScript::NumericConstantData& data) noexcept
        {
            Logger::log("NUMBER ", data.value, '\n');
        }

        inline void handle_variable_declaration_data(const RaychelScript::VariableDeclarationData& data) noexcept
        {
            Logger::log("VAR_REF ", data.is_const ? "CONST " : "MUT ", data.name, '\n');
        }

        inline void handle_variable_reference_data(const RaychelScript::VariableReferenceData& data) noexcept
        {
            Logger::log("VAR ", data.name, '\n');
        }

        inline void handle_arithmetic_expression_data(const RaychelScript::ArithmeticExpressionData& data) noexcept
        {
            using Op = RaychelScript::ArithmeticExpressionData::Operation;
            switch (data.operation) {
                case Op::add:
                    Logger::log("ADD\n");
                    break;
                case Op::subtract:
                    Logger::log("SUBTRACT\n");
                    break;
                case Op::multiply:
                    Logger::log("MULTIPLY\n");
                    break;
                case Op::divide:
                    Logger::log("DIVIDE\n");
                    break;
                case Op::power:
                    Logger::log("POWER\n");
                    break;
                default:
                    RAYCHEL_ASSERT_NOT_REACHED;
            }
            print_node(data.lhs, "lhs=");
            print_node(data.rhs, "rhs=");
        }

        inline void handle_unary_operator_data(const RaychelScript::UnaryExpressionData& data) noexcept
        {
            using Op = RaychelScript::UnaryExpressionData::Operation;
            switch (data.operation) {
                case Op::minus:
                    Logger::log("UNARY MINUS\n");
                    break;
                case Op::plus:
                    Logger::log("UNARY PLUS\n");
                    break;
                case Op::factorial:
                    Logger::log("FACTORIAL\n");
                    break;
                case Op::magnitude:
                    Logger::log("MAGNITUDE\n");
                    break;
                default:
                    RAYCHEL_ASSERT_NOT_REACHED;
            }
            print_node(data.value_node, "expression=");
        }

        inline void handle_conditional_construct(const RaychelScript::ConditionalConstructData& data) noexcept
        {
            Logger::log("CONDITIONAL\n");

            print_node(data.condition_node, "condition=");

            for (const auto& node : data.body) {
                print_node(node, "body=");
            }
        }

        inline void handle_relational_operator(const RaychelScript::RelationalOperatorData& data) noexcept
        {
            using Op = RaychelScript::RelationalOperatorData::Operation;
            switch (data.operation) {
                case Op::equals:
                    Logger::log("EQUALS\n");
                    break;
                case Op::not_equals:
                    Logger::log("NOT EQUALS\n");
                    break;
                case Op::less_than:
                    Logger::log("LESS THAN\n");
                    break;
                case Op::greater_than:
                    Logger::log("GREATER THAN\n");
                    break;
                default:
                    RAYCHEL_ASSERT_NOT_REACHED;
            }

            print_node(data.lhs, "lhs=");
            print_node(data.rhs, "rhs=");
        }

        inline void handle_loop(const LoopData& data) noexcept
        {
            Logger::log("LOOP\n");
            print_node(data.condition_node, "condition=");

            for (const auto& body_node : data.body) {
                print_node(body_node, "body=");
            }
        }

    } // namespace details

    inline void print_config_block(const RaychelScript::ConfigBlock& config_block) noexcept
    {
        if (config_block.input_identifiers.empty()) {
            Logger::log("input vars:\n");
            for (const auto& input_id : config_block.input_identifiers) {
                Logger::log('\t', input_id, '\n');
            }
        }

        if (config_block.output_identifiers.empty()) {
            Logger::log("output vars:\n");
            for (const auto& output_id : config_block.output_identifiers) {
                Logger::log('\t', output_id, '\n');
            }
        }

        if (!config_block.config_vars.empty()) {
            Logger::log("config vars:\n");
            for (const auto& [name, vars] : config_block.config_vars) {
                Logger::log('\t', name, ": ");
                for (const auto& var_name : vars) {
                    Logger::log(var_name, ", ");
                }
                Logger::log('\n');
            }
        }
    }

    inline void print_node(const RaychelScript::AST_Node& node, std::string_view prefix) noexcept
    {
        namespace RS = RaychelScript;

        RS::IndentHandler handler;

        Logger::log(handler.indent(), prefix);

        switch (node.type()) {
            case RS::NodeType::assignment:
                details::handle_assignment_data(node.to_node_data<RS::AssignmentExpressionData>());
                break;

            case RS::NodeType::variable_decl:
                details::handle_variable_declaration_data(node.to_node_data<RS::VariableDeclarationData>());
                break;

            case RS::NodeType::variable_ref:
                details::handle_variable_reference_data(node.to_node_data<RS::VariableReferenceData>());
                break;

            case RS::NodeType::arithmetic_operator:
                details::handle_arithmetic_expression_data(node.to_node_data<RS::ArithmeticExpressionData>());
                break;

            case RS::NodeType::numeric_constant:
                details::handle_numeric_constant_data(node.to_node_data<RS::NumericConstantData>());
                break;

            case RS::NodeType::unary_operator:
                details::handle_unary_operator_data(node.to_node_data<RS::UnaryExpressionData>());
                break;

            case RS::NodeType::conditional_construct:
                details::handle_conditional_construct(node.to_node_data<RS::ConditionalConstructData>());
                break;

            case RS::NodeType::literal_true:
                Logger::log("TRUE\n");
                break;

            case RS::NodeType::literal_false:
                Logger::log("FALSE\n");
                break;

            case RS::NodeType::relational_operator:
                details::handle_relational_operator(node.to_node_data<RS::RelationalOperatorData>());
                break;

            case RS::NodeType::inline_state_push:
                Logger::log("INLINE STATE PUSH\n");
                break;

            case RS::NodeType::inline_state_pop:
                Logger::log("INLINE STATE POP\n");
                break;

            case RS::NodeType::loop:
                details::handle_loop(node.to_node_data<LoopData>());
        }
    }

    inline void pretty_print_ast(const AST& ast) noexcept
    {
        if (ast.nodes.empty()) {
            Logger::log("<Empty>\n");
        }

        RaychelScript::IndentHandler::reset_indent();

        for_each_top_node(ast, [](auto& node) { print_node(node, ""); });
    }

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_PRINT_AST_H