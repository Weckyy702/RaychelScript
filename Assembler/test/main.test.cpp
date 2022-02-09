#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"
#include "rasm/WritePipe.h"

#include "rasm/read.h"
int main(int argc, char** argv)
{
    using namespace RaychelScript::Pipes; //NOLINT

    const auto script_name = [&]() -> std::string {
        if (argc > 1) {
            return argv[1]; //NOLINT
        }
        return "script.rsc";
    }();
    const auto output_name = [&]() -> std::string {
        if (argc > 2) {
            return argv[2]; //NOLINT
        }
        return "out.rsbf";
    }();

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    {
        const auto ast_or_error = Lex{lex_file, script_name} | Parse{};

        const auto label = Logger::startTimer("Assembling time");
        const auto data_or_error = ast_or_error | Assemble{};
        Logger::logDuration<std::chrono::microseconds>(label);

        if (log_if_error(data_or_error)) {
            return 1;
        }

        if (log_if_error(data_or_error | Write{output_name})) {
            return 1;
        }
    }

    {
        const auto data_or_error = PipeResult<RaychelScript::VM::VMData>{RaychelScript::Assembly::read_rsbf(output_name)};

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
    }
}