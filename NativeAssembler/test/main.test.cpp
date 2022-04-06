#include "NativeAssembler/NativeAssembler.h"

#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3) {
        Logger::error("Usage: ", argv[0], " <INPUT_FILE> <OUTPUT_FILE>\n");
        return 1;
    }
    using namespace RaychelScript::Pipes;

    const auto data_or_error = Lex{lex_file, argv[1]} | Parse{} | Assemble{};
    if (log_if_error(data_or_error)) {
        return 1;
    }

    const auto data = data_or_error.value();

    std::ofstream output_stream{argv[2]};
    const auto ec = assemble(data, RaychelScript::NativeAssembler::assemble_x86_64, output_stream);

    if (ec != RaychelScript::NativeAssembler::NativeAssemblerErrorCode::ok) {
        Logger::error(ec, '\n');
    }
    return 0;
}
