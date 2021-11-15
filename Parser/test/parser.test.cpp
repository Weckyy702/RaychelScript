#include "Parser.h"

#include <RaychelCore/AssertingGet.h>
#include <fstream>
#include <string_view>
#include <vector>

#include "IndentHandler.h"
#include "NodeData.h"

static void print_config_block(const RaychelScript::ConfigBlock& config_block) noexcept
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

static void print_node(const RaychelScript::AST_Node& node, std::string_view prefix) noexcept;

static void handle_assignment_data(const RaychelScript::AssignmentExpressionData& data) noexcept
{
    Logger::log("ASSIGN\n");
    print_node(data.lhs, "lhs=");
    print_node(data.rhs, "rhs=");
}

static void handle_numeric_constant_data(const RaychelScript::NumericConstantData& data) noexcept
{
    Logger::log("NUMBER ", data.value, '\n');
}

static void handle_variable_declaration_data(const RaychelScript::VariableDeclarationData& data) noexcept
{
    Logger::log("VAR_REF ", data.is_const ? "CONST " : "MUT ", data.name, '\n');
}

static void handle_variable_reference_data(const RaychelScript::VariableReferenceData& data) noexcept
{
    Logger::log("VAR ", data.name, '\n');
}

static void handle_arithmetic_expression_data(const RaychelScript::ArithmeticExpressionData& data) noexcept
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

static void print_node(const RaychelScript::AST_Node& node, std::string_view prefix) noexcept
{
    using RaychelScript::IndentHandler, RaychelScript::NodeType, RaychelScript::AssignmentExpressionData,
        RaychelScript::VariableDeclarationData, RaychelScript::VariableDeclarationData, RaychelScript::VariableReferenceData,
        RaychelScript::ArithmeticExpressionData, RaychelScript::NumericConstantData;

    IndentHandler handler;

    Logger::log(handler.indent(), prefix);

    switch (node.type()) {
        case NodeType::assignment:
            handle_assignment_data(node.to_node_data<AssignmentExpressionData>());
            break;

        case NodeType::variable_decl:
            handle_variable_declaration_data(node.to_node_data<VariableDeclarationData>());
            break;

        case NodeType::variable_ref:
            handle_variable_reference_data(node.to_node_data<VariableReferenceData>());
            break;

        case NodeType::math_op:
            handle_arithmetic_expression_data(node.to_node_data<ArithmeticExpressionData>());
            break;

        case NodeType::numeric_constant:
            handle_numeric_constant_data(node.to_node_data<NumericConstantData>());
            break;
        default:
            RAYCHEL_ASSERT_NOT_REACHED;
    }
}

static void pretty_print_ast(const std::vector<RaychelScript::AST_Node>& nodes) noexcept
{
    if (nodes.empty()) {
        Logger::log("<Empty>\n");
    }

    RaychelScript::IndentHandler::reset_indent();

    for (const auto& node : nodes) {
        print_node(node, "");
    }
}

[[maybe_unused]] static void parse_file_and_print_debug_info(const std::string& filename) noexcept
{
    Logger::info("Parsing file '", filename, "'\n");
    std::ifstream in_file{"../../../shared/test/" + filename};

    const auto label = Logger::startTimer("parse time");
    const auto res = RaychelScript::parse(in_file);
    Logger::logDuration(label);

    if (const RaychelScript::AST* ast = std::get_if<RaychelScript::AST>(&res); ast) {
        print_config_block(ast->config_block);
        pretty_print_ast(ast->nodes);
    } else {
        Logger::log(
            "The following error occured during parsing: ",
            error_code_to_reason_string(Raychel::get<RaychelScript::ParserErrorCode>(res)),
            '\n');
    }
}

[[maybe_unused]] static void echo_AST_from_stdin() noexcept
{
    Logger::setMinimumLogLevel(Logger::LogLevel::info);
    std::string line;

    do {
        RaychelScript::IndentHandler::reset_indent();

        std::cout << ">>";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        const auto AST_or_error = RaychelScript::_parse_no_config_check(line);

        if (const auto* ec = std::get_if<RaychelScript::ParserErrorCode>(&AST_or_error); ec) {
            Logger::log("<ERROR>: ", error_code_to_reason_string(*ec), '\n');
            continue;
        }

        const auto ast = Raychel::get<RaychelScript::AST>(AST_or_error);

        pretty_print_ast(ast.nodes);

    } while (true);
}

int main(int /*argc*/, char** /*argv*/)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    // parse_file_and_print_debug_info("abc.rsc");
    // parse_file_and_print_debug_info("unmatched_paren.rsc");
    // parse_file_and_print_debug_info("floats.rsc");
    // // parse_file_and_print_debug_info("large.rsc");
    // parse_file_and_print_debug_info("test.rsc");
    // parse_file_and_print_debug_info("invalid_config.rsc");
    // parse_file_and_print_debug_info("config_tests.rsc");

    parse_file_and_print_debug_info("parentheses.rsc");

    // RaychelScript::parse(R"(
    //     [[config]]
    //     name alsdkjfsdaklj
    //     input x, y, z, r
    //     output d

    //     [[body]]
    //     d = (x^2 + y^2 + z^2)^0.5 - 2
    // )");

    echo_AST_from_stdin();
}