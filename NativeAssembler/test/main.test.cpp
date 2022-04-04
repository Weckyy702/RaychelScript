#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"

#include "NativeAssembler/NativeAssembler.h"

int main()
{
    using namespace RaychelScript::Pipes;
    const auto data_or_error = Lex{lex_file, "./abc.rsc"} | Parse{} | Assemble{};

    if (log_if_error(data_or_error)) {
        return 1;
    }
    const auto data = data_or_error.value();

    const auto ec = RaychelScript::NativeAssembler::assemble(data, RaychelScript::NativeAssembler::assemble_x86_64, std::cout);

    if (ec != RaychelScript::NativeAssembler::NativeAssemblerErrorCode::ok) {
        Logger::error(ec, '\n');
    }
    return 0;
}
