#include "Interpreter.h"
#include "AST/NodeData.h"

#include <algorithm>
#include <cerrno>
#include <cfenv>
#include <cmath>

#include "RaychelMath/equivalent.h"
#include "RaychelMath/math.h"

#define RAYCHELSCRIPT_INTERPRETER_SILENT 1

#define RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(name)                                                                 \
    template <typename T>                                                                                                        \
    [[nodiscard]] InterpreterErrorCode handle_##name(ExecutionState<T>& state, const AST_Node& node) noexcept

#if !RAYCHELSCRIPT_INTERPRETER_SILENT
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...) Logger::debug(__VA_ARGS__)
#else
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...)
#endif

namespace RaychelScript::Interpreter {

    //helper functions

    template <typename T>
    [[nodiscard]] bool has_identifier(const ExecutionState<T>& state, const std::string& name) noexcept
    {
        return state._descriptor_table.find(name) != state._descriptor_table.end();
    }

    template <typename T>
    void add_constant(ExecutionState<T>& state, ConstantDescriptor<T>&& descriptor, const std::string& name) noexcept
    {
        state._descriptor_table.insert({name, descriptor.id()});
        state.constants.push_back(std::move(descriptor));
    }

    template <typename T>
    void add_variable(ExecutionState<T>& state, VariableDescriptor<T>&& descriptor, const std::string& name) noexcept
    {
        state._descriptor_table.insert({name, descriptor.id()});
        state.variables.push_back(std::move(descriptor));
    }

    template <typename T>
    void clear_value_registers(ExecutionState<T>& state) noexcept
    {
        state._registers.a = state._registers.b = 0;
        state._registers.result = 0;
    }

    template <typename T>
    void clear_status_registers(ExecutionState<T>& state) noexcept
    {
        state._registers.flags = StateFlags::none;
    }

    template <typename T>
    void set_status_registers(ExecutionState<T>& state) noexcept
    {
        clear_status_registers(state);
        if (Raychel::equivalent<T>(state._registers.result, 0)) {
            state._registers.flags |= StateFlags::zero;
        }

        if (state._registers.result < 0) {
            state._registers.flags |= StateFlags::negative;
        }
    }

    template <typename T>
    void set_descriptor_index(ExecutionState<T>& state, const DescriptorID& id) noexcept
    {
        state._current_descriptor = id;
    }

    template <typename T>
    std::pair<bool, std::size_t> get_descriptor_index(ExecutionState<T>& state, const std::string& name) noexcept
    {
        const auto id = state._descriptor_table.at(name);

        return {id.is_constant(), id.index()};
    }

    template <typename T>
    InterpreterErrorCode do_assign(ExecutionState<T>& state) noexcept
    {
        const auto value = state._registers.result;

        RAYCHELSCRIPT_INTERPRETER_DEBUG(
            "Assigning value ",
            value,
            " to ",
            state._current_descriptor.is_constant() ? "constant" : "variable",
            " descriptor at index ",
            state._current_descriptor.index(),
            '\n');

        if (state._current_descriptor.is_constant()) {
            auto& descriptor = state.constants.at(state._current_descriptor.index());

            if (descriptor.has_value()) {
                Logger::error("Assigning to already-initialized constant!\n");
                return InterpreterErrorCode::constant_reassign;
            }

            descriptor.set_value(value);
        } else {
            auto& descriptor = state.variables.at(state._current_descriptor.index());

            descriptor.value() = value;
        }

        clear_value_registers(state);

        return InterpreterErrorCode::ok;
    }

    template <typename T>
    InterpreterErrorCode do_factorial(ExecutionState<T>& state)
    {

        const auto value = state._registers.result;

        if (Raychel::is_integer(value) && value < 0) {
            Logger::error("Cannot compute factorial of negative integer value!\n");
            return InterpreterErrorCode::invalid_argument;
        }

        if (Raychel::equivalent<T>(value, 0)) {
            state._registers.result = 1;
        } else {
            state._registers.result = std::tgamma(value + 1);
        }
        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    //setup functions

    template <typename T>
    [[nodiscard]] InterpreterErrorCode populate_input_descriptors(
        ExecutionState<T>& state, const AST& ast, const std::map<std::string, T>& input_identifiers) noexcept
    {
        if (ast.config_block.input_identifiers.size() != input_identifiers.size()) {
            Logger::error(
                "Number of input identifiers does not match! Expected ",
                ast.config_block.input_identifiers.size(),
                ", got ",
                input_identifiers.size(),
                '\n');
            return InterpreterErrorCode::not_enough_input_identifiers;
        }

        InterpreterErrorCode result{};

        //TODO: find a better algorithm to do this
        std::for_each(
            ast.config_block.input_identifiers.begin(), ast.config_block.input_identifiers.end(), [&](const auto& identifier) {
                if (result != InterpreterErrorCode::ok) {
                    return;
                }

                if (has_identifier(state, identifier)) {
                    Logger::error("An identifier with name '", identifier, "' already exists!\n");
                    result = InterpreterErrorCode::duplicate_name;
                    return;
                }

                if (const auto it = input_identifiers.find(identifier); it != input_identifiers.end()) {
                    auto descriptor = ConstantDescriptor<T>{it->second};
                    RAYCHELSCRIPT_INTERPRETER_DEBUG(
                        "Adding empty constant descriptor with id=", descriptor.id(), ", value=", descriptor.value(), '\n');

                    state._descriptor_table.insert({identifier, descriptor.id()});
                    state.constants.push_back(std::move(descriptor));
                } else {
                    Logger::error("Input identifier '", identifier, "' has no value assigned!\n");
                    result = InterpreterErrorCode::invalid_input_identifier;
                }
            });

        return result;
    }

    template <typename T>
    [[nodiscard]] InterpreterErrorCode populate_output_descriptors(ExecutionState<T>& state, const AST& ast) noexcept
    {
        for (const auto& name : ast.config_block.output_identifiers) {

            if (has_identifier(state, name)) {
                Logger::error("An identifier with name '", name, "' already exists!\n");
                return InterpreterErrorCode::duplicate_name;
            }

            VariableDescriptor<T> descriptor;

            RAYCHELSCRIPT_INTERPRETER_DEBUG("Adding output variable descriptor with id=", descriptor.id(), '\n');

            state._descriptor_table.insert({name, descriptor.id()});
            state.variables.push_back(std::move(descriptor));
        }
        return InterpreterErrorCode::ok;
    }

    template <typename T>
    [[nodiscard]] InterpreterErrorCode handle_config_vars(ExecutionState<T>& state, const AST& ast) noexcept
    {
        //TODO: parse configuration variables and change  state flags if needed
        (void)state;

        for (const auto& [name, values] : ast.config_block.config_vars) {
            RAYCHELSCRIPT_INTERPRETER_DEBUG("Got configuration variable(s) with name '", name, "':\n");
            for ([[maybe_unused]] const auto& value : values) {
                RAYCHELSCRIPT_INTERPRETER_DEBUG("\t'", value, "'\n");
            }
        }

        return InterpreterErrorCode::ok;
    }

    //handler functions and main interpreter loop

    template <typename T>
    [[nodiscard]] InterpreterErrorCode execute_node(ExecutionState<T>& state, const AST_Node& node) noexcept;

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(assignment_node)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_assignment_node()\n");
        const auto data = node.to_node_data<AssignmentExpressionData>();

        if (!data.lhs.is_value_reference()) {
            Logger::error("Left hand side of assignment expression is not a value reference!\n");
            return InterpreterErrorCode::invalid_node; //TODO: this should have its own error code
        }

        //TODO: find a better way to solve this issue. handle_variable_reference has different meaning based on which side of the assignment expression the node is on
        state._load_references = true;
        //We need to execute the rhs first in case it sets the descriptor index
        if (const auto ec = execute_node(state, data.rhs); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        state._load_references = false;
        if (const auto ec = execute_node(state, data.lhs); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        return do_assign(state);
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(variable_declaration)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_variable_declaration()\n");

        const auto data = node.to_node_data<VariableDeclarationData>();

        if (has_identifier(state, data.name)) {
            Logger::error("Duplicate variable name '", data.name, "'!\n");
            return InterpreterErrorCode::duplicate_name;
        }

        if (data.is_const) {
            auto descriptor = ConstantDescriptor<T>{};

            RAYCHELSCRIPT_INTERPRETER_DEBUG(
                "Adding new constant descriptor with name '", data.name, "' and id ", descriptor.id(), '\n');

            set_descriptor_index(state, descriptor.id());
            add_constant(state, std::move(descriptor), data.name);
        } else {
            auto descriptor = VariableDescriptor<T>{};

            RAYCHELSCRIPT_INTERPRETER_DEBUG(
                "Adding new variable descriptor with name '", data.name, "' and id ", descriptor.id(), '\n');

            set_descriptor_index(state, descriptor.id());
            add_variable(state, std::move(descriptor), data.name);
        }

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(variable_reference)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_variable_reference(): ", state._load_references ? "LOAD" : "STORE", '\n');

        const auto data = node.to_node_data<VariableReferenceData>();

        if (!has_identifier(state, data.name)) {
            Logger::error("Unable to resolve identifier '", data.name, "'!\n");
            return InterpreterErrorCode::unresolved_identifier;
        };

        if (!state._load_references) {
            const auto id = state._descriptor_table.at(data.name);

            set_descriptor_index(state, id);

            return InterpreterErrorCode::ok;
        }

        const auto [is_constant, index] = get_descriptor_index(state, data.name);

        if (is_constant) {
            state._registers.result = state.constants.at(index).value();
        } else {
            state._registers.result = state.variables.at(index).value();
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(arithmetic_operation)
    {
        using Op = ArithmeticExpressionData::Operation;
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_arithmetic_operation()\n");

        const auto data = node.to_node_data<ArithmeticExpressionData>();

        state._load_references = true;

        if (const auto ec = execute_node(state, data.lhs); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        state._registers.a = state._registers.result;

        if (const auto ec = execute_node(state, data.rhs); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        state._registers.b = state._registers.result;

        switch (data.operation) {
            case Op::add:
                state._registers.result = state._registers.a + state._registers.b;
                break;
            case Op::subtract:
                state._registers.result = state._registers.a - state._registers.b;
                break;
            case Op::multiply:
                state._registers.result = state._registers.a * state._registers.b;
                break;
            case Op::divide:
                if (Raychel::equivalent<T>(state._registers.b, 0)) {
                    return InterpreterErrorCode::divide_by_zero;
                }
                state._registers.result = state._registers.a / state._registers.b;
                break;
            case Op::power:
                state._registers.result = std::pow(state._registers.a, state._registers.b);
                break;
            default:
                return InterpreterErrorCode::invalid_arithmetic_operation;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(numeric_constant)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_numeric_constant()\n");

        const auto data = node.to_node_data<NumericConstantData>();

        state._registers.result = data.value;

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(unary_expression)
    {
        using Op = UnaryExpressionData::Operation;

        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_unary_expression()\n");

        const auto data = node.to_node_data<UnaryExpressionData>();

        state._load_references = true;
        if (const auto ec = execute_node(state, data.value); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        switch (data.operation) {
            case Op::minus:
                state._registers.result = -state._registers.result;
                break;
            case Op::plus:
                //do nothing
                break;
            case Op::factorial:
                return do_factorial(state);
                break;
            case Op::magnitude:
                state._registers.result = std::abs(state._registers.result);
                break;
            default:
                return InterpreterErrorCode::invalid_arithmetic_operation;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    template <typename T>
    [[nodiscard]] InterpreterErrorCode execute_node(ExecutionState<T>& state, const AST_Node& node) noexcept
    {
        switch (node.type()) {
            case NodeType::assignment:
                return handle_assignment_node(state, node);
            case NodeType::variable_decl:
                return handle_variable_declaration(state, node);
            case NodeType::variable_ref:
                return handle_variable_reference(state, node);
            case NodeType::arithmetic_operator:
                return handle_arithmetic_operation(state, node);
            case NodeType::numeric_constant:
                return handle_numeric_constant(state, node);
            case NodeType::unary_operator:
                return handle_unary_expression(state, node);
        }

        return InterpreterErrorCode::invalid_node;
    }

    //Interpreter entry point

    [[nodiscard]] ExecutionResult<double> interpret(const AST& ast, const std::map<std::string, double>& parameters) noexcept
    {
        ExecutionState<double> state;

        DescriptorID::reset_id<ConstantDescriptor<double>>();
        DescriptorID::reset_id<VariableDescriptor<double>>();

        if (const auto ec = populate_input_descriptors(state, ast, parameters); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        if (const auto ec = populate_output_descriptors(state, ast); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        if (const auto ec = handle_config_vars(state, ast); ec != InterpreterErrorCode::ok) {
            return ec;
        }

        for (const auto& node : ast.nodes) {
            clear_value_registers(state);
            clear_status_registers(state);
            state._load_references = true;

            const auto ec = execute_node(state, node);
            if (ec != InterpreterErrorCode::ok) {
                return ec;
            }
        }

        return state;
    }

} // namespace RaychelScript::Interpreter
