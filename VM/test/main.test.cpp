#include "VM/VM.h"

#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"
#include "VM/VMPipe.h"
#include "rasm/ReadPipe.h"

#include "RaychelCore/AssertingGet.h"

int main(int argc, char** argv)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    std::string script_name{"script.rsc"};
    std::vector<double> args{1, 2};

    //NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    if (argc > 2) {
        script_name = argv[1];

        args.clear();
        for (int i = 2; i < argc; i++) {
            char* end{};
            const double arg = std::strtod(argv[i], &end);
            if (end != argv[i]) {
                args.emplace_back(arg);
            }
        }
    }

    const auto is_binary_file = script_name.ends_with(".rsbf");

    //NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    Logger::info("Executing ", is_binary_file ? "binary " : "script ", script_name, '\n');

    const auto data_or_error = [&]() -> RaychelScript::Pipes::PipeResult<RaychelScript::Assembly::VMData> {
        using namespace RaychelScript::Pipes;
        if (is_binary_file) {
            return ReadRSBF{"./instr.rsbf"};
        }
        return Lex{lex_file, script_name} | Parse{} | Assemble{};
    }();

    const auto state_or_error = data_or_error | RaychelScript::Pipes::Execute<double>{std::move(args)};

    if (log_if_error(state_or_error)) {
        return 1;
    }

    const auto [data, state] = state_or_error.value();

    const auto output = RaychelScript::VM::get_output_variables(state, data);
    for (const auto& [name, value] : output) {
        Logger::info(name, " = ", value, '\n');
    }

    return 0;
}