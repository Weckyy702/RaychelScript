#include "Interpreter.h"

#include "RaychelCore/AssertingGet.h"

#include "Parser.h"

int main()
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    auto input_file = std::ifstream{"../../../shared/test/abc.rsc"};

    const auto state_or_error_code = RaychelScript::Interpreter::interpret(input_file, {
        {"a", 1},
        {"b", 1}
    });

    if (const auto* ec = std::get_if<RaychelScript::Interpreter::InterpreterErrorCode>(&state_or_error_code); ec) {
        Logger::error("Error during execution! Reason: ", RaychelScript::Interpreter::error_code_to_reason_string(*ec), '\n');
    } else {
        Logger::info("SUCCESS :)\n");

        const auto state = Raychel::get<RaychelScript::ExecutionState<double>>(state_or_error_code);

        Logger::log("Constant values: \n");
        for (const auto& descriptor : state.constants) {
            Logger::log('\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
        }
        Logger::log("Variable values: \n");
        for (const auto& descriptor : state.variables) {
            Logger::log('\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
        }
    }
}