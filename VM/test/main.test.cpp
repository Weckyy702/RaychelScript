#include "VM/VM.h"

#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Optimizer/OptimizerPipe.h"
#include "Parser/ParserPipe.h"
#include "VM/VMPipe.h"

#include "RaychelCore/AssertingGet.h"

int main(int argc, char** argv)
{
    using namespace RaychelScript::Pipes;

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    std::string script_name{"../../../shared/test/loops.rsc"};
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

    //NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    Logger::info("Executing script ", script_name, '\n');

    const auto state_or_error = Lex{lex_file, script_name} | Parse{} |
                                Optimize{RaychelScript::Optimizer::OptimizationLevel::hard} | Assemble{} |
                                Execute<double>{std::move(args)};

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