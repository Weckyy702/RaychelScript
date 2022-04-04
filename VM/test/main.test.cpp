#define RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING 1
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

    const auto file_name = [&]() -> std::string {
        if (argc > 1) {
            return argv[1]; //NOLINT
        }
        return "script.rsc";
    }();

    const auto args = [&]() -> std::vector<double> {
        if (argc > 2) {
            std::vector<double> _args{};
            for (int i = 2; i < argc; i++) {
                char* end{};
                const double arg = std::strtod(argv[i], &end); //NOLINT
                if (end != argv[i]) {                          //NOLINT
                    _args.emplace_back(arg);
                }
            }
            return _args;
        }
        return {1, 2, 3};
    }();

    const auto is_binary_file = file_name.ends_with(".rsbf");

    Logger::info("Executing ", is_binary_file ? "binary " : "script ", file_name, '\n');

    const auto data_or_error = [&]() -> RaychelScript::Pipes::PipeResult<RaychelScript::VM::VMData> {
        using namespace RaychelScript::Pipes; //NOLINT
        if (is_binary_file) {
            return ReadRSBF{file_name};
        }
        return Lex{{}, file_name} | Parse{} | Assemble{};
    }();

    if (log_if_error(data_or_error)) {
        return 1;
    }

    const auto state_or_error = data_or_error | RaychelScript::Pipes::Execute<double>{args};

    if (log_if_error(state_or_error)) {
        return 1;
    }

    const auto [data, state] = state_or_error.value();

    for (const auto value : state.memory) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    return 0;
}
