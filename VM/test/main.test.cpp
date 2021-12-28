#include "VM/VM.h"

#include "Assembler/AssemblerPipe.h"
#include "Interpreter/Interpreter.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"

#include "RaychelCore/AssertingGet.h"

int main()
{
    using namespace RaychelScript::Pipes;

    const auto ast_or_error = Lex{lex_file, "../../../shared/test/simple.rsc"} | Parse{};

    if (log_if_error(ast_or_error)) {
        return 1;
    }

    const auto data_or_error = ast_or_error | Assemble{};

    if (log_if_error(data_or_error)) {
        return 1;
    }

    const auto data = data_or_error.value();

    Logger::log("Input identifiers:\n");
    for (const auto& [identifier, address] : data.config_block.input_identifiers) {
        Logger::info(address, " -> ", identifier, '\n');
    }

    Logger::log("Output identifiers:\n");
    for (const auto& [identifier, address] : data.config_block.output_identifiers) {
        Logger::info(address, " -> ", identifier, '\n');
    }

    Logger::log("Immediate values:\n");
    for (const auto& [value, address] : data.immediate_values) {
        Logger::info(address, " -> ", value, '\n');
    }

    Logger::log("Instructions:\n");
    std::size_t index{0};
    for (const auto instr : data.instructions) {
        Logger::info(index++, ": ", instr, '\n');
    }

    const auto state_or_error = RaychelScript::VM::execute<double>(data_or_error.value(), {1, 2, 3});

    if (const auto* ec = std::get_if<RaychelScript::VM::VMErrorCode>(&state_or_error); ec) {
        Logger::log(*ec, '\n');
        return 1;
    }

    const auto state = Raychel::get<RaychelScript::VM::VMState<double>>(state_or_error);

    index = 0;

    for (const auto& value : state.memory) {
        Logger::log('$', index++, ": ", value, '\n');
    }

    return 0;
}