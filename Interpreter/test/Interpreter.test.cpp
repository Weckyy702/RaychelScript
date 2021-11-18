#include "Interpreter.h"

#include "RaychelCore/AssertingGet.h"

#include "Parser.h"

int main()
{
    const auto ast_or_error_code = RaychelScript::Parser::parse(R"raw_string(
        [[config]]
            name execution_test
            input a, b
            output d

            use_strict_mode
            exit_on_fperror
        [[body]]
            let d = a + b
            d *= 2
            c = d
    )raw_string");

    if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&ast_or_error_code); ec) {
        Logger::error("Error during parsing: ", RaychelScript::Parser::error_code_to_reason_string(*ec), '\n');
    }

    const auto ast = Raychel::get<RaychelScript::AST>(ast_or_error_code);

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    const auto state_or_error_code = RaychelScript::Interpreter::interpret(
        ast,
        {
            {"a", 12},
            {"b", 0},
        });

    if (const auto* ec = std::get_if<RaychelScript::ExecutionErrorCode>(&state_or_error_code); ec) {
        Logger::error("Error during execution!\n");
    } else {
        Logger::info("SUCCESS :)\n");

        const auto state = Raychel::get<RaychelScript::ExecutionState<double>>(state_or_error_code);

        for (const auto& descriptor : state.variables) {
            Logger::log(descriptor.id() , ": ", descriptor.value(), '\n');
        }
    }
}