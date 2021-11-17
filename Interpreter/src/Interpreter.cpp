#include "Interpreter.h"

#include <algorithm>

namespace RaychelScript::Interpreter {

    template <typename T>
    [[nodiscard]] ExecutionErrorCode populate_input_descriptors(
        ExecutionState<T>& state, const AST& ast, const std::map<std::string, T>& input_identifiers) noexcept
    {
        if (ast.config_block.input_identifiers.size() != input_identifiers.size()) {
            Logger::error(
                "Number of input identifiers does not match! Expected ",
                ast.config_block.input_identifiers.size(),
                ", got ",
                input_identifiers.size(),
                '\n');
            return ExecutionErrorCode::not_enough_input_identifiers;
        }

        ExecutionErrorCode error_code{ExecutionErrorCode::ok};

        //TODO: find a better algorithm to do this
        std::for_each(
            ast.config_block.input_identifiers.begin(), ast.config_block.input_identifiers.end(), [&](const auto& identifier) {
                if (error_code != ExecutionErrorCode::ok) {
                    return;
                }

                if (const auto it = std::find_if(
                        input_identifiers.begin(),
                        input_identifiers.end(),
                        [&](const auto& pair) { return pair.first == identifier; });
                    it != input_identifiers.end()) {
                        auto descriptor = ConstantDescriptor{it->second};
                        Logger::debug("Adding constant descriptor with id=", descriptor.id(), ", value=", descriptor.value(), '\n');

                        state.descriptor_table.insert({identifier, descriptor.id()});
                        state.input_vars.push_back(std::move(descriptor));
                } else {
                    Logger::error("Input identifier '", identifier, "' has no value assigned!\n");
                    error_code = ExecutionErrorCode::invalid_input_identifier;
                }
            });

        return error_code;
    }

    [[nodiscard]] ExecutionResult<double> interpret(const AST& ast, const std::map<std::string, double>& input_identifiers) noexcept
    {
        ExecutionState<double> state;

        if (const auto ec = populate_input_descriptors(state, ast, input_identifiers); ec != ExecutionErrorCode::ok) {
            return ec;
        }

        for (const auto& descriptor : state.input_vars) {
            Logger::debug(descriptor.id(), " - ", descriptor.value(), '\n');

            const auto it = std::find_if(state.descriptor_table.begin(), state.descriptor_table.end(), [&](const auto& pair) {
                return pair.second == descriptor.id();
            });
            RAYCHEL_ASSERT(it != state.descriptor_table.end());

            Logger::debug("Descriptor table entry: name=\"", it->first, "\" id=", it->second, '\n');
        }

        return state;
    }

} // namespace RaychelScript::Interpreter
