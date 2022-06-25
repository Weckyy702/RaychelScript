/**
* \file Interpreter.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation for Interpreter functions
* \date 2021-12-04
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

#include "Interpreter/Interpreter.h"
#include "shared/AST/NodeData.h"

#include <algorithm>
#include <cerrno>
#include <cfenv>
#include <cmath>

#include "RaychelCore/ClassMacros.h"

#include "RaychelMath/equivalent.h"
#include "RaychelMath/math.h"

#define RAYCHELSCRIPT_INTERPRETER_SILENT 0

#define RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(name)                                                                 \
    [[nodiscard]] InterpreterErrorCode handle_##name(State& state, const AST_Node& node) noexcept

#define TRY(expression)                                                                                                          \
    if (const auto ec = (expression); ec != InterpreterErrorCode::ok) {                                                          \
        return ec;                                                                                                               \
    }

#if !RAYCHELSCRIPT_INTERPRETER_SILENT
constexpr std::string_view debug_indent{
    "...................................................................................................."};
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...) Logger::debug(debug_indent.substr(0, state.indent * 2), __VA_ARGS__)
#else
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...)
#endif

namespace RaychelScript::Interpreter {

    enum class IterationDecision {
        continue_,
        break_,
    };

    template <std::invocable<const State::Scope&> F>
    bool for_each_scope(const State& state, F&& f) noexcept
    {
        auto scope_it = std::prev(state.scopes.end());
        do {
            const auto& scope = *scope_it;

            if (f(scope) != IterationDecision::continue_) {
                return true;
            }

            if (!scope.inherits_from_parent_scope) {
                break; //we need to stop traversing the scope chain here
            }
        } while (scope_it-- != state.scopes.begin());
        return false;
    }

    //helper functions
    [[nodiscard]] static bool has_identifier(const State& state, const std::string& name) noexcept
    {
        return for_each_scope(state, [&](const State::Scope& scope) {
            if (scope.descriptor_table.find(name) != scope.descriptor_table.end()) {
                return IterationDecision::break_;
            }
            return IterationDecision::continue_;
        });
    }

    [[nodiscard]] static std::optional<details::ValueData> find_identifier(const State& state, const std::string& name) noexcept
    {
        std::optional<details::ValueData> maybe_descriptor{};
        for_each_scope(state, [&](const State::Scope& scope) {
            if (const auto it = scope.descriptor_table.find(name); it != scope.descriptor_table.end()) {
                maybe_descriptor = it->second;
                return IterationDecision::break_;
            }
            return IterationDecision::continue_;
        });

        return maybe_descriptor;
    }

    [[nodiscard]] static double get_descriptor_value(State& state, details::ValueData descriptor) noexcept
    {
        if (descriptor.is_constant) {
            return state.scopes.at(descriptor.index_in_scope_chain).constants.at(descriptor.index_in_scope).value_or(0);
        }
        return state.scopes.at(descriptor.index_in_scope_chain).variables.at(descriptor.index_in_scope);
    }

    details::ValueData
    add_constant(State& state, const std::string& name, std::optional<double> initial_value = std::nullopt) noexcept
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("Adding new constant with name='", name, "', value=", initial_value.value_or(0.0), '\n');
        auto& current_scope = state.scopes.back();
        auto [it, _] = current_scope.descriptor_table.insert(
            {name, details::ValueData{current_scope.constants.size(), state.scopes.size() - 1, true}});
        current_scope.constants.push_back(initial_value);

        return it->second;
    }

    details::ValueData add_variable(State& state, const std::string& name) noexcept
    {
        auto& current_scope = state.scopes.back();
        auto [it, _] = current_scope.descriptor_table.insert(
            {name, details::ValueData{current_scope.variables.size(), state.scopes.size() - 1, false}});
        current_scope.variables.push_back(0.0);

        return it->second;
    }

    void clear_value_registers(State& state) noexcept
    {
        state.registers.a = state.registers.b = 0;
        state.registers.result = 0;
        state._current_descriptor.reset();
    }

    void clear_status_registers(State& state) noexcept
    {
        state.registers.flags = StateFlags::none;
    }

    void set_status_registers(State& state) noexcept
    {
        clear_status_registers(state);
        if (Raychel::equivalent(state.registers.result, 0.0)) {
            state.registers.flags |= StateFlags::zero;
        }

        if (state.registers.result < 0) {
            state.registers.flags |= StateFlags::negative;
        }
    }

    InterpreterErrorCode do_assign(State& state) noexcept
    {
        const auto value = state.registers.result;

        if (!state._current_descriptor.has_value()) {
            Logger::error("BUG: current state desciptor is empty!\n");
            return InterpreterErrorCode::no_input;
        }

        RAYCHELSCRIPT_INTERPRETER_DEBUG(
            "Assigning value ",
            value,
            " to ",
            state._current_descriptor->is_constant ? "constant" : "variable",
            " descriptor at index ",
            state._current_descriptor->index_in_scope_chain,
            ':',
            state._current_descriptor->index_in_scope,
            '\n');

        if (state._current_descriptor->is_constant) {
            auto& constant = state.scopes.back().constants.at(state._current_descriptor->index_in_scope);

            if (constant.has_value()) {
                Logger::error("Assigning to already-initialized constant!\n");
                return InterpreterErrorCode::constant_reassign;
            }

            constant = value;
        } else {
            auto& variable = state.scopes.back().variables.at(state._current_descriptor->index_in_scope);

            variable = value;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    InterpreterErrorCode do_factorial(State& state) noexcept
    {
        const auto value = state.registers.result;

        if (state.registers.flags & StateFlags::negative && Raychel::is_integer(value)) {
            Logger::error("Cannot compute factorial of negative integer value!\n");
            return InterpreterErrorCode::invalid_argument;
        }

        if (state.registers.flags & StateFlags::zero) {
            state.registers.result = 1;
        } else {
            state.registers.result = std::tgamma(value + 1);
        }
        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    void push_scope(State& state, bool scope_inherits_from_parent, std::string_view name) noexcept
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("push_scope(): ", name, '\n');
        state.scopes.push_back(State::Scope{scope_inherits_from_parent});
    }

    InterpreterErrorCode pop_scope(State& state, const std::string& name) noexcept
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("pop_scope(): ", name, '\n');

        if (state.scopes.size() == 1) {
            Logger::error("Cannot pop global scope!\n");
            return InterpreterErrorCode::pop_empy_stack;
        }
        state.scopes.pop_back();
        state._current_descriptor.reset();

        return InterpreterErrorCode::ok;
    }

    namespace details {
        /**
        * \brief RAII wrapper around the push/pop_scope functions
        */
        class PushScope
        {

            RAYCHEL_MAKE_NONCOPY_NONMOVE(PushScope)

        public:
            explicit PushScope(State& state, std::string_view name, bool scope_inherits_from_parent = true)
                : state_{state}, name_{name}
            {
                push_scope(state_, scope_inherits_from_parent, name);
                ++state.indent;
            }

            ~PushScope() noexcept
            {
                --state_.indent;
                RAYCHEL_ASSERT(pop_scope(state_, name_) == InterpreterErrorCode::ok);
            }

        private:
            State& state_;
            std::string name_;
        };
    } // namespace details

    //setup functions

    [[nodiscard]] InterpreterErrorCode
    populate_input_descriptors(State& state, const AST& ast, const std::map<std::string, double>& input_identifiers) noexcept
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

                    add_constant(state, it->first, it->second);
                } else {
                    Logger::error("Input identifier '", identifier, "' has no value assigned!\n");
                    result = InterpreterErrorCode::invalid_input_identifier;
                }
            });

        return result;
    }

    [[nodiscard]] InterpreterErrorCode populate_output_descriptors(State& state, const AST& ast) noexcept
    {
        for (const auto& name : ast.config_block.output_identifiers) {

            if (has_identifier(state, name)) {
                Logger::error("An identifier with name '", name, "' already exists!\n");
                return InterpreterErrorCode::duplicate_name;
            }

            RAYCHELSCRIPT_INTERPRETER_DEBUG("Adding output variable descriptor with name '", name, "'\n");

            add_variable(state, name);
        }
        return InterpreterErrorCode::ok;
    }

    [[nodiscard]] InterpreterErrorCode handle_config_vars(State& state, const AST& ast) noexcept
    {
        //TODO: parse configuration variables and change state flags if needed
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

    [[nodiscard]] InterpreterErrorCode execute_node(State& state, const AST_Node& node) noexcept;

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(assignment_node)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_assignment_node()\n");
        const auto data = node.to_node_data<AssignmentExpressionData>();

        state._load_references = true;
        TRY(execute_node(state, data.rhs));

        state._load_references = false;
        TRY(execute_node(state, data.lhs));

        return do_assign(state);
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(variable_declaration)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_variable_declaration()\n");

        const auto data = node.to_node_data<VariableDeclarationData>();

        if (has_identifier(state, data.name)) {
            Logger::error("Duplicate identifier '", data.name, "'!\n");
            return InterpreterErrorCode::duplicate_name;
        }

        if (data.is_const) {

            RAYCHELSCRIPT_INTERPRETER_DEBUG("Adding new constant descriptor with name '", data.name, "'\n");

            state._current_descriptor = add_constant(state, data.name);

            return InterpreterErrorCode::ok;
        }

        RAYCHELSCRIPT_INTERPRETER_DEBUG("Adding new variable descriptor with name '", data.name, "'\n");

        state._current_descriptor = add_variable(state, data.name);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(variable_reference)
    {
        const auto data = node.to_node_data<VariableReferenceData>();

        RAYCHELSCRIPT_INTERPRETER_DEBUG(
            "handle_variable_reference(): ", state._load_references ? "LOAD " : "STORE ", data.name, '\n');

        const auto maybe_identifier = find_identifier(state, data.name);

        if (!maybe_identifier.has_value()) {
            Logger::error("Cannot resolve identifier '", data.name, "'\n");
            return InterpreterErrorCode::unresolved_identifier;
        }

        if (!state._load_references) {
            state._current_descriptor = maybe_identifier.value();
            return InterpreterErrorCode::ok;
        }

        state.registers.result = get_descriptor_value(state, maybe_identifier.value());

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(arithmetic_operation)
    {
        using Op = ArithmeticExpressionData::Operation;
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_arithmetic_operation()\n");

        const auto data = node.to_node_data<ArithmeticExpressionData>();

        state._load_references = true;

        TRY(execute_node(state, data.lhs));

        state.registers.a = state.registers.result;

        state._load_references = true;
        TRY(execute_node(state, data.rhs));

        state.registers.b = state.registers.result;

        switch (data.operation) {
            case Op::add:
                state.registers.result = state.registers.a + state.registers.b;
                break;
            case Op::subtract:
                state.registers.result = state.registers.a - state.registers.b;
                break;
            case Op::multiply:
                state.registers.result = state.registers.a * state.registers.b;
                break;
            case Op::divide:
                if (state.registers.flags & StateFlags::zero) {
                    return InterpreterErrorCode::divide_by_zero;
                }
                state.registers.result = state.registers.a / state.registers.b;
                break;
            case Op::power:
                state.registers.result = std::pow(state.registers.a, state.registers.b);
                break;
            default:
                return InterpreterErrorCode::invalid_arithmetic_operation;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(update_expression)
    {
        using Op = UpdateExpressionData::Operation;
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_update_expression()\n");

        const auto data = node.to_node_data<UpdateExpressionData>();

        state._load_references = true;
        TRY(execute_node(state, data.rhs));

        state._load_references = false;
        TRY(execute_node(state, data.lhs));

        if (state._current_descriptor->is_constant) {
            Logger::error("Trying to update a constant!\n");
            return InterpreterErrorCode::constant_reassign;
        }

        double& value = state.scopes.at(state._current_descriptor->index_in_scope_chain)
                            .variables.at(state._current_descriptor->index_in_scope);

        switch (data.operation) {
            case Op::add:
                value += state.registers.result;
                break;
            case Op::subtract:
                value -= state.registers.result;
                break;
            case Op::multiply:
                value *= state.registers.result;
                break;
            case Op::divide:
                if (Raychel::equivalent(state.registers.result, 0.0)) {
                    return InterpreterErrorCode::divide_by_zero;
                }
                value /= state.registers.result;
                break;
            case Op::power:
                value = std::pow(value, state.registers.result);
                break;
            default:
                return InterpreterErrorCode::invalid_arithmetic_operation;
        }

        state.registers.result = value;

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(numeric_constant)
    {
        const auto data = node.to_node_data<NumericConstantData>();
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_numeric_constant(): ", data.value, '\n');

        state.registers.result = static_cast<double>(data.value);

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(unary_expression)
    {
        using Op = UnaryExpressionData::Operation;

        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_unary_expression()\n");

        const auto data = node.to_node_data<UnaryExpressionData>();

        state._load_references = true;
        TRY(execute_node(state, data.value_node));

        switch (data.operation) {
            case Op::minus:
                state.registers.result = -state.registers.result;
                break;
            case Op::plus:
                //do nothing
                break;
            case Op::factorial:
                return do_factorial(state);
            case Op::magnitude:
                state.registers.result = std::abs(state.registers.result);
                break;
            default:
                return InterpreterErrorCode::invalid_arithmetic_operation;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(conditional_construct)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_conditional_construct()\n");

        const auto data = node.to_node_data<ConditionalConstructData>();

        state._load_references = true;
        TRY(execute_node(state, data.condition_node));

        const auto* nodes_to_execute = &data.else_body;

        if (state.registers.flags == StateFlags::condition_was_true) {
            nodes_to_execute = &data.body;
        }

        if (nodes_to_execute->empty()) {
            return InterpreterErrorCode::ok;
        }

        RAYCHEL_ANON_VAR details::PushScope{state, "if"};

        for (const auto& body_node : *nodes_to_execute) {
            state._load_references = true;
            TRY(execute_node(state, body_node));
        }

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(relational_operator)
    {
        using Op = RelationalOperatorData::Operation;
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_relational_construct()\n");

        const auto data = node.to_node_data<RelationalOperatorData>();

        state._load_references = true;
        TRY(execute_node(state, data.lhs));
        state.registers.a = state.registers.result;

        TRY(execute_node(state, data.rhs));
        state.registers.b = state.registers.result;

        switch (data.operation) {
            case Op::equals:
                state.registers.flags =
                    Raychel::equivalent(state.registers.a, state.registers.b) ? StateFlags::condition_was_true : StateFlags::none;
                break;
            case Op::not_equals:
                state.registers.flags = !Raychel::equivalent(state.registers.a, state.registers.b)
                                            ? StateFlags::condition_was_true
                                            : StateFlags::none;
                break;
            case Op::less_than:
                state.registers.flags = state.registers.a < state.registers.b ? StateFlags::condition_was_true : StateFlags::none;
                break;
            case Op::greater_than:
                state.registers.flags = state.registers.a > state.registers.b ? StateFlags::condition_was_true : StateFlags::none;
                break;
            default:
                return InterpreterErrorCode::invalid_relational_operation;
        }

        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(loop)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_loop()\n");

        const auto data = node.to_node_data<LoopData>();

        while (true) {
            TRY(execute_node(state, data.condition_node));

            if (state.registers.flags != StateFlags::condition_was_true) {
                return InterpreterErrorCode::ok;
            }

            RAYCHEL_ANON_VAR details::PushScope{state, "loop"};

            for (const auto& body_node : data.body) {
                state._load_references = true;
                TRY(execute_node(state, body_node));
            }
        }
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(function_call)
    {
        [[maybe_unused]] const auto data = node.to_node_data<FunctionCallData>();

        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_function_call(): ", data.mangled_callee_name, '\n');

        //Checks
        if (!state.ast.functions.contains(data.mangled_callee_name)) {
            Logger::error("Cannot find function with mangled name '", data.mangled_callee_name, "'!\n");
            return InterpreterErrorCode::unresolved_identifier;
        }

        const auto& function_data = state.ast.functions.at(data.mangled_callee_name);

        if (data.argument_expressions.size() != function_data.arguments.size()) {
            Logger::error(
                "Wrong number of arguments supplied to function '",
                function_data.mangled_name,
                "'! Expected ",
                function_data.arguments.size(),
                ", got ",
                data.argument_expressions.size(),
                '\n');
            return InterpreterErrorCode::invalid_argument;
        }

        //Evaluate the argument expressions in the parent scope
        std::vector<std::pair<std::string, double>> argument_values(function_data.arguments.size());

        for (const auto& [name, index] : function_data.arguments) {
            const auto& argument_node = data.argument_expressions.at(index);

            state._load_references = true;
            TRY(execute_node(state, argument_node));
            argument_values.at(index) = std::make_pair(name, state.registers.result);
        }

        RAYCHEL_ANON_VAR details::PushScope{state, data.mangled_callee_name, false};

        //Push all arguments into the function scope
        for (const auto& [name, value] : argument_values) {
            add_constant(state, name, value);
        }

        for (const auto& child_node : function_data.body) {
            TRY(execute_node(state, child_node));
            if (state.registers.flags & StateFlags::return_from_function) {
                break;
            }
        }

        state.registers.flags = StateFlags::none;
        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(return )
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_return()\n");

        const auto data = node.to_node_data<FunctionReturnData>();

        state._load_references = true;
        TRY(execute_node(state, data.return_value));
        state.registers.flags |= StateFlags::return_from_function;

        return InterpreterErrorCode::ok;
    }

    [[nodiscard]] InterpreterErrorCode execute_node(State& state, const AST_Node& node) noexcept
    {
        //skip executing all nodes until we leave the function
        if (state.registers.flags & StateFlags::return_from_function) {
            Logger::debug("\033[95mSkipping node\n");
            return InterpreterErrorCode::ok;
        }

        switch (node.type()) {
            case NodeType::assignment:
                return handle_assignment_node(state, node);
            case NodeType::variable_decl:
                return handle_variable_declaration(state, node);
            case NodeType::variable_ref:
                return handle_variable_reference(state, node);
            case NodeType::arithmetic_operator:
                return handle_arithmetic_operation(state, node);
            case NodeType::update_expression:
                return handle_update_expression(state, node);
            case NodeType::numeric_constant:
                return handle_numeric_constant(state, node);
            case NodeType::unary_operator:
                return handle_unary_expression(state, node);
            case NodeType::conditional_construct:
                return handle_conditional_construct(state, node);
            case NodeType::relational_operator:
                return handle_relational_operator(state, node);
            case NodeType::inline_state_push:
            case NodeType::inline_state_pop:
                break;
            case NodeType::loop:
                return handle_loop(state, node);
            case NodeType::function_call:
                return handle_function_call(state, node);
            case NodeType::function_return:
                return handle_return(state, node);
        }

        return InterpreterErrorCode::invalid_node;
    }

    //Interpreter entry point

    [[nodiscard]] Interpreter::ExecutionResult interpret(const AST& ast, const std::map<std::string, double>& parameters) noexcept
    {
        State state{.ast = ast};

        state.scopes.push_back(
            State::Scope{.inherits_from_parent_scope = false, .constants = {}, .variables = {}, .descriptor_table = {}});

        TRY(populate_input_descriptors(state, ast, parameters));

        TRY(populate_output_descriptors(state, ast));

        TRY(handle_config_vars(state, ast));

        for (const auto& node : ast.nodes) {
            clear_value_registers(state);
            clear_status_registers(state);
            state._load_references = true;

            TRY(execute_node(state, node));
        }

        return state;
    }

} // namespace RaychelScript::Interpreter
