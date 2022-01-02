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

#define RAYCHELSCRIPT_INTERPRETER_SILENT 1

#define RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(name)                                                                 \
    template <typename T>                                                                                                        \
    [[nodiscard]] InterpreterErrorCode handle_##name(State<T>& state, const AST_Node& node) noexcept

#define TRY(expression)                                                                                                          \
    if (const auto ec = (expression); ec != InterpreterErrorCode::ok) {                                                          \
        return ec;                                                                                                               \
    }

#if !RAYCHELSCRIPT_INTERPRETER_SILENT
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...) Logger::debug(__VA_ARGS__)
#else
    #define RAYCHELSCRIPT_INTERPRETER_DEBUG(...)
#endif

namespace RaychelScript::Interpreter {

    template <typename T>
    using State = InterpreterState<ConstantDescriptor<T>, VariableDescriptor<T>>;

    //helper functions
    template <typename T>
    [[nodiscard]] bool has_identifier(const State<T>& state, const std::string& name) noexcept
    {
        return state._descriptor_table.find(name) != state._descriptor_table.end();
    }

    template <typename T>
    void add_constant(State<T>& state, ConstantDescriptor<T>&& descriptor, const std::string& name) noexcept
    {
        state._descriptor_table.insert({name, descriptor.id()});
        state.constants.push_back(std::move(descriptor));
    }

    template <typename T>
    void add_variable(State<T>& state, VariableDescriptor<T>&& descriptor, const std::string& name) noexcept
    {
        state._descriptor_table.insert({name, descriptor.id()});
        state.variables.push_back(std::move(descriptor));
    }

    template <typename T>
    void clear_value_registers(State<T>& state) noexcept
    {
        state.registers.a = state.registers.b = 0;
        state.registers.result = 0;
    }

    template <typename T>
    void clear_status_registers(State<T>& state) noexcept
    {
        state.registers.flags = StateFlags::none;
        state._current_descriptor = DescriptorID{};
    }

    template <typename T>
    void set_status_registers(State<T>& state) noexcept
    {
        clear_status_registers(state);
        if (Raychel::equivalent<T>(state.registers.result, 0)) {
            state.registers.flags |= StateFlags::zero;
        }

        if (state.registers.result < 0) {
            state.registers.flags |= StateFlags::negative;
        }
    }

    template <typename T>
    void set_descriptor_index(State<T>& state, const DescriptorID& id) noexcept
    {
        state._current_descriptor = id;
    }

    template <typename T>
    std::pair<bool, std::size_t> get_descriptor_index(State<T>& state, const std::string& name) noexcept
    {
        const auto id = state._descriptor_table.at(name);

        return {id.is_constant(), id.index()};
    }

    template <typename T>
    InterpreterErrorCode do_assign(State<T>& state) noexcept
    {
        const auto value = state.registers.result;

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

            if (descriptor.has_value_set()) {
                Logger::error("Assigning to already-initialized constant!\n");
                return InterpreterErrorCode::constant_reassign;
            }

            descriptor.set_value(value);
        } else {
            auto& descriptor = state.variables.at(state._current_descriptor.index());

            descriptor.value() = value;
        }

        set_status_registers(state);

        return InterpreterErrorCode::ok;
    }

    template <typename T>
    InterpreterErrorCode do_factorial(State<T>& state) noexcept
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

    template <typename T>
    void push_state(State<T>& state) noexcept
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("push_state()\n");
        state._stack_snapshots.push({state.constants.size(), state.variables.size()});
    }

    template <Descriptor Desc>
    void do_descriptor_relocate(
        std::vector<Desc>& descriptors, std::unordered_map<std::string, DescriptorID>& descriptor_table,
        std::size_t descriptor_index) noexcept
    {
        for (std::size_t i = descriptor_index; i != descriptors.size(); i++) {
            const auto& id = descriptors.at(i).id();
            std::erase_if(descriptor_table, [&id](const auto& entry) { return entry.second == id; });
        }

        descriptors.erase(descriptors.begin() + static_cast<std::ptrdiff_t>(descriptor_index), descriptors.end());
        DescriptorID::reset_id<Desc>(descriptors.back().id().id() + 1);
    }

    template <typename T>
    InterpreterErrorCode pop_state(State<T>& state) noexcept
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("pop_state()\n");
        if (state._stack_snapshots.empty()) {
            Logger::error("Tried to pop empty stack!\n");
            return InterpreterErrorCode::pop_empy_stack;
        }

        const auto [constant_index, variable_index] = state._stack_snapshots.top();

        do_descriptor_relocate(state.constants, state._descriptor_table, constant_index);
        do_descriptor_relocate(state.variables, state._descriptor_table, variable_index);

        return InterpreterErrorCode::ok;
    }

    namespace details {
        /**
        * \brief RAII wrapper around the push/pop_state functions
        */
        template <std::floating_point T>
        class PushState
        {

            RAYCHEL_MAKE_NONCOPY_NONMOVE(PushState)

        public:
            explicit PushState(State<T>& state) : state_{state}
            {
                push_state(state_);
            }

            ~PushState() noexcept
            {
                RAYCHEL_ANON_VAR pop_state(state_);
            }

        private:
            State<T>& state_;
        };
    } // namespace details

    //setup functions

    template <typename T>
    [[nodiscard]] InterpreterErrorCode
    populate_input_descriptors(State<T>& state, const AST& ast, const std::map<std::string, T>& input_identifiers) noexcept
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
                        "Adding new constant descriptor with id=", descriptor.id(), ", value=", descriptor.value(), '\n');

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
    [[nodiscard]] InterpreterErrorCode populate_output_descriptors(State<T>& state, const AST& ast) noexcept
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
    [[nodiscard]] InterpreterErrorCode handle_config_vars(State<T>& state, const AST& ast) noexcept
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
    [[nodiscard]] InterpreterErrorCode execute_node(State<T>& state, const AST_Node& node) noexcept;

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
            Logger::error("Duplicate variable name '", data.name, "'!\n");
            return InterpreterErrorCode::duplicate_name;
        }

        if (data.is_const) {
            auto descriptor = ConstantDescriptor<T>{};

            RAYCHELSCRIPT_INTERPRETER_DEBUG(
                "Adding new constant descriptor with name '", data.name, "' and id ", descriptor.id(), '\n');

            set_descriptor_index(state, descriptor.id());
            add_constant(state, std::move(descriptor), data.name);

            return InterpreterErrorCode::ok;
        }
        auto descriptor = VariableDescriptor<T>{};

        RAYCHELSCRIPT_INTERPRETER_DEBUG(
            "Adding new variable descriptor with name '", data.name, "' and id ", descriptor.id(), '\n');

        set_descriptor_index(state, descriptor.id());
        add_variable(state, std::move(descriptor), data.name);

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
            state.registers.result = state.constants.at(index).value();
        } else {
            state.registers.result = state.variables.at(index).value();
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

        TRY(execute_node(state, data.lhs));

        state.registers.a = state.registers.result;

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

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(numeric_constant)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_numeric_constant()\n");

        const auto data = node.to_node_data<NumericConstantData>();

        state.registers.result = static_cast<T>(data.value);

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

        if (state.registers.flags & StateFlags::zero) {
            return InterpreterErrorCode::ok;
        }

        push_state(state);

        for (const auto& body_node : data.body) {
            clear_value_registers(state);
            clear_status_registers(state);
            TRY(execute_node(state, body_node));
        }

        TRY(pop_state(state));

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
                state.registers.result = Raychel::equivalent<T>(state.registers.a, state.registers.b);
                break;
            case Op::not_equals:
                state.registers.result = !Raychel::equivalent<T>(state.registers.a, state.registers.b);
                break;
            case Op::less_than:
                state.registers.result = state.registers.a < state.registers.b;
                break;
            case Op::greater_than:
                state.registers.result = state.registers.a > state.registers.b;
                break;
            default:
                return InterpreterErrorCode::invalid_relational_operation;
        }

        set_status_registers(state);
        return InterpreterErrorCode::ok;
    }

    RAYCHELSCRIPT_INTERPRETER_DEFINE_NODE_HANDLER_FUNC(loop)
    {
        RAYCHELSCRIPT_INTERPRETER_DEBUG("handle_loop()\n");

        const auto data = node.to_node_data<LoopData>();

        while (true) {
            TRY(execute_node(state, data.condition_node));

            if (state.registers.flags & StateFlags::zero) {
                return InterpreterErrorCode::ok;
            }

            RAYCHEL_ANON_VAR details::PushState{state};

            for (const auto& body_node : data.body) {
                clear_value_registers(state);
                clear_status_registers(state);
                TRY(execute_node(state, body_node));
            }
        }
    }

    template <typename T>
    [[nodiscard]] InterpreterErrorCode execute_node(State<T>& state, const AST_Node& node) noexcept
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
            case NodeType::conditional_construct:
                return handle_conditional_construct(state, node);
            case NodeType::literal_true:
                state.registers.result = 1;
                return InterpreterErrorCode::ok;
            case NodeType::literal_false:
                state.registers.result = 0;
                state.registers.flags |= StateFlags::zero;
                return InterpreterErrorCode::ok;
            case NodeType::relational_operator:
                return handle_relational_operator(state, node);
            case NodeType::inline_state_push:
                push_state(state);
                return InterpreterErrorCode::ok;
            case NodeType::inline_state_pop:
                return pop_state(state);
            case NodeType::loop:
                return handle_loop(state, node);
        }

        return InterpreterErrorCode::invalid_node;
    }

    //Interpreter entry point

    template <std::floating_point T>
    [[nodiscard]] Interpreter::ExecutionResult<T> interpret(const AST& ast, const std::map<std::string, T>& parameters) noexcept
    {
        State<T> state;

        DescriptorID::reset_id<ConstantDescriptor<T>>();
        DescriptorID::reset_id<VariableDescriptor<T>>();

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

    namespace details {
        /**
        * \brief Very evil hack to instantiate the Interpreter entry points for all floating-point types
        * 
        */
        void _instantiate_interpreter_entry_point()
        {
            [[maybe_unused]] auto _float = interpret<float>(AST{}, {});
            [[maybe_unused]] auto _double = interpret<double>(AST{}, {});
            [[maybe_unused]] auto _long_double = interpret<long double>(AST{}, {});
        }
    }; // namespace details

} // namespace RaychelScript::Interpreter
