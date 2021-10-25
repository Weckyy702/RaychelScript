#include "Parser.h"

#include <RaychelCore/AssertingGet.h>
#include <fstream>
#include <string_view>
#include <vector>

#include "IndentHandler.h"
#include "NodeData.h"

constexpr std::string_view error_code_to_reason_string(RaychelScript::ParserErrorCode ec) noexcept
{
    using namespace std::string_view_literals;
    using RaychelScript::ParserErrorCode;

    switch (ec) {
        case ParserErrorCode::no_input:
            return "No Input"sv;
        case ParserErrorCode::invalid_config:
            return "Error in configuration block"sv;
        case ParserErrorCode::invalid_construct:
            return "Invalid construct"sv;
        case ParserErrorCode::invalid_declaration:
            return "Invalid variable declaration"sv;
        case ParserErrorCode::invalid_numeric_constant:
            return "Invalid numeric constant"sv;
    }
    return "<Unknown reason>"sv;
}

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

static void print_node(const RaychelScript::AST_Node&, std::string_view) noexcept;

static void handle_assignment_data(const RaychelScript::AssignmentExpressionData& data) noexcept
{
    Logger::log("ASSIGN\n");
    print_node(data.lhs, "lhs=");
    print_node(data.rhs, "rhs=");
}

static void handle_numeric_constant_data(const RaychelScript::NumericConstantData& data) noexcept
{
    using namespace RaychelScript;

    Logger::log(data.value, '\n');
}

static void handle_variable_declaration_data(const RaychelScript::VariableDeclarationData& data) noexcept
{
    using namespace RaychelScript;

    Logger::log(data.is_const ? "CONST " : "MUT ", data.name, '\n');
}

static void handle_variable_reference_data(const RaychelScript::VariableReferenceData& data) noexcept
{
    using namespace RaychelScript;

    Logger::log(data.name, '\n');
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
        default:
            RAYCHEL_ASSERT_NOT_REACHED;
    }
    print_node(data.lhs, "lhs=");
    print_node(data.rhs, "rhs=");
}

static void print_node(const RaychelScript::AST_Node& node, std::string_view prefix) noexcept
{
    using namespace RaychelScript;

    IndentHandler handler;

    Logger::log(handler.indent(), prefix);

    switch (node.type()) {
        case NodeType::assignment:
            handle_assignment_data(node.to_node_data<AssignmentExpressionData>());
            break;

        case NodeType::numeric_constant:
            handle_numeric_constant_data(node.to_node_data<NumericConstantData>());
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

static void parse_file_and_print_debug_info(const std::string& filename) noexcept
{
    Logger::log(filename, '\n');
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

int main(int /*argc*/, char** /*argv*/)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    parse_file_and_print_debug_info("abc.rsc");
    //parse_file_and_print_debug_info("unmatched_paren.rsc");
    //parse_file_and_print_debug_info("floats.rsc");
    //parse_file_and_print_debug_info("large.rsc");
    //parse_file_and_print_debug_info("test.rsc");
    //parse_file_and_print_debug_info("invalid_config.rsc");
    //parse_file_and_print_debug_info("config_tests.rsc");
}