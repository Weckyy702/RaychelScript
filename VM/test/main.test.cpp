#include "VM/VM.h"

#include "Assembler/AssemblerPipe.h"
#include "Interpreter/Interpreter.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"

#include "RaychelCore/AssertingGet.h"

int main(int argc, char** argv)
{
    using namespace RaychelScript::Pipes;

    std::string script_name{"../../../shared/test/abc.rsc"};
    std::vector<double> args{1, 2};

    //NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    if (argc > 2) {
        script_name = argv[1];

        args.clear();
        args.reserve(argc - 2);
        for (int i = 2; i < argc; i++) {
            char* end{};
            const double arg = std::strtod(argv[i], &end);
            if (end != argv[i]) {
                args.emplace_back(arg);
            }
        }
    }

    //NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    Logger::info("Executing script ", script_name, '\n');

    const auto data_or_error = Lex{lex_file, script_name} | Parse{} | Assemble{};

    if (log_if_error(data_or_error)) {
        return 1;
    }

    const auto data = data_or_error.value();
    const auto state_or_error = RaychelScript::VM::execute<double>(data, args);

    if (const auto* ec = std::get_if<RaychelScript::VM::VMErrorCode>(&state_or_error); ec) {
        Logger::log(*ec, '\n');
        return 1;
    }

    const auto state = Raychel::get<RaychelScript::VM::VMState<double>>(state_or_error);

    std::size_t index{};

    for (const auto& value : state.memory) {
        Logger::log('$', index++, ": ", value, '\n');
    }

    const auto output = RaychelScript::VM::get_output_variables(state, data);
    for (const auto& [name, value] : output) {
        Logger::info(name, " = ", value, '\n');
    }

    return 0;
}