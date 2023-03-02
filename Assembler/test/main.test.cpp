#include "Assembler/AssemblerPipe.h"

#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"
#include "rasm/write.h"

int main(int argc, char** argv)
{
    using namespace RaychelScript::Pipes; //NOLINT

    const auto script_name = [&]() -> std::string {
        if (argc > 1) {
            return argv[1]; //NOLINT
        }
        return "script.rsc";
    }();

    const auto output_filename = [&]() -> std::string {
        if (argc > 2) {
            return argv[2]; //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        return {};
    }();

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    auto data_or_error = Lex{lex_file, script_name} | Parse{} | Assemble{};
    if (log_if_error(data_or_error)) {
        return 1;
    }
    const auto data = std::move(data_or_error).value();

    Logger::log<std::size_t, const char*>(data.num_input_identifiers, " inputs\n");

    Logger::log("Immediates:\n");
    std::size_t immediate_index{};
    for (const auto& immediate_value : data.immediate_values) {
        Logger::log("  %", immediate_index++, " = ", immediate_value, '\n');
    }

    std::size_t frame_index{};
    for (const auto& frame : data.call_frames) {
        Logger::log("Frame #", frame_index++, " with memory size ", static_cast<int>(frame.size), ":\n");
        std::size_t instruction_index{};
        for (const auto& instr : frame.instructions) {
            Logger::log('%', instruction_index++, ": ", instr, '\n');
        }
    }

    if (output_filename.empty())
        return 0;

    Logger::log("Writing output to '", output_filename, "'\n");

    return RaychelScript::Assembly::write_rsbf(output_filename, data) ? 0 : 1;
}
