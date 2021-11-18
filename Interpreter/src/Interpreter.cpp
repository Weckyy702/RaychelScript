#include "Interpreter.h"

#include <algorithm>

namespace RaychelScript::Interpreter {

    template <typename T>
    [[nodiscard]] auto find_input_identifier(const std::string& name, const std::map<std::string, T>& identifiers) noexcept
    {
        return std::find_if(identifiers.begin(), identifiers.end(), [&](const auto& pair) { return pair.first == name; });
    }

    template <typename T>
    [[nodiscard]] bool has_identifier(const ExecutionState<T>& state, const std::string& name) noexcept
    {
        return state._descriptor_table.find(name) != state._descriptor_table.end();
    }

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

        ExecutionErrorCode result{};

        //TODO: find a better algorithm to do this
        std::for_each(
            ast.config_block.input_identifiers.begin(), ast.config_block.input_identifiers.end(), [&](const auto& identifier) {
                if (result != ExecutionErrorCode::ok) {
                    return;
                }

                if (has_identifier(state, identifier)) {
                    Logger::error("An identifier with name '", identifier, "' already exists!\n");
                    result = ExecutionErrorCode::duplicate_name;
                    return;
                }

                if (const auto it = find_input_identifier(identifier, input_identifiers); it != input_identifiers.end()) {
                    auto descriptor = ConstantDescriptor{it->second};
                    Logger::debug("Adding constant descriptor with id=", descriptor.id(), ", value=", descriptor.value(), '\n');

                    state._descriptor_table.insert({identifier, descriptor.id()});
                    state.constants.push_back(std::move(descriptor));
                } else {
                    Logger::error("Input identifier '", identifier, "' has no value assigned!\n");
                    result = ExecutionErrorCode::invalid_input_identifier;
                }
            });

        return result;
    }

    template <typename T>
    [[nodiscard]] ExecutionErrorCode populate_output_descriptors(ExecutionState<T>& state, const AST& ast) noexcept
    {
        for (const auto& name : ast.config_block.output_identifiers) {
            VariableDescriptor<T> descriptor;

            if (has_identifier(state, name)) {
                Logger::error("An identifier with name '", name, "' already exists!\n");
                return ExecutionErrorCode::duplicate_name;
            }

            Logger::debug("Adding output variable descriptor with id=", descriptor.id(), '\n');

            state._descriptor_table.insert({name, descriptor.id()});
            state.variables.push_back(std::move(descriptor));
        }
        return ExecutionErrorCode::ok;
    }

    template <typename T>
    [[nodiscard]] ExecutionErrorCode handle_config_vars(ExecutionState<T>& state, const AST& ast) noexcept
    {
        //TODO: parse configuration variables and change  state flags if needed
        (void)state;

        for (const auto& [name, values] : ast.config_block.config_vars) {
            Logger::debug("Got configuration variable(s) with name '", name, "':\n");
            for (const auto& value : values) {
                Logger::debug("\t'", value, "'\n");
            }
        }

        return ExecutionErrorCode::ok;
    }

    [[nodiscard]] ExecutionResult<double>
    interpret(const AST& ast, const std::map<std::string, double>& input_identifiers) noexcept
    {
        ExecutionState<double> state;

        if (const auto ec = populate_input_descriptors(state, ast, input_identifiers); ec != ExecutionErrorCode::ok) {
            return ec;
        }

        if (const auto ec = populate_output_descriptors(state, ast); ec != ExecutionErrorCode::ok) {
            return ec;
        }

        if (const auto ec = handle_config_vars(state, ast); ec != ExecutionErrorCode::ok) {
            return ec;
        }

        return state;
    }

} // namespace RaychelScript::Interpreter
