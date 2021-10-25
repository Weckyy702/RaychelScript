#include "Lexer.h"
#include <fstream>
#include "RaychelLogger/Logger.h"

static std::string to_token_name(RaychelScript::TokenType::TokenType type) noexcept
{
    using namespace RaychelScript::TokenType;
    using namespace std::string_literals;
    switch (type) {
        case identifer:
            return "IDENTIFIER"s;
        case number:
            return "NUMBER"s;
        case declaration:
            return "DECL"s;
        default:
            return {type};
    }
}

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
    // )");

    // Logger::setLogColor(Logger::LogLevel::debug, "\033[31;m");

    std::ifstream in_file{"../../../shared/test/abc.rsc"};
    const auto lines = RaychelScript::lex(in_file);

    if (!lines.has_value()) {
        return EXIT_FAILURE;
    }

    std::size_t line_no = 1U;

    for (const auto& line : *lines) {
        Logger::log(line_no, ": ");
        for (const auto& [type, _, __] : line) {
            Logger::log(to_token_name(type), ' ');
        }
        Logger::log('\n');
        line_no++;
    }
}