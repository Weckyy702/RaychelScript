#include "Lexer.h"
#include <fstream>
#include "RaychelLogger/Logger.h"
int main(int /*unused*/, char** /*unused*/)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    // RaychelScript::lex(R"(
    //     [[config]]
    //     name alsdkjfsdaklj
    //     input x, y, z, r
    //     output d

    //     [[body]]
    //     d = sqrt(x^2 + y^2 + z^2) - 2
    //)");

    // Logger::setLogColor(Logger::LogLevel::debug, "\033[31;m");

    std::ifstream in_file{"../../../shared/test/conditionals.rsc"};
    const auto lines = RaychelScript::Lexer::lex(in_file);

    if (!lines.has_value()) {
        return EXIT_FAILURE;
    }

    std::size_t line_no = 1U;

    for (const auto& line : *lines) {
        Logger::log(line_no, ": ");
        for (const auto& [type, _, __] : line) {
            Logger::log(RaychelScript::token_type_to_string(type), ' ');
        }
        Logger::log('\n');
        line_no++;
    }
}