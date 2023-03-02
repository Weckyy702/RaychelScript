/**
* \file parser.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for parser testing executable
* \date 2021-12-04
*
* MIT License
* Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/
#include "Parser/ParserPipe.h"

#include <RaychelCore/AssertingGet.h>
#include <fstream>
#include <string_view>
#include <vector>

#include "shared/AST/NodeData.h"
#include "shared/IndentHandler.h"
#include "shared/Misc/PrintAST.h"

[[maybe_unused]] static void parse_file_and_print_debug_info(const std::string& file_path) noexcept
{
    Logger::info("Parsing file '", file_path, "'\n");
    std::ifstream in_file{};

    const auto label = Logger::startTimer("parse time");

    const auto res = RaychelScript::Pipes::Lex(RaychelScript::Pipes::lex_file, file_path) | RaychelScript::Pipes::Parse{};

    Logger::logDuration<std::chrono::microseconds>(label);

    if (log_if_error(res)) {
        return;
    }
    const auto& ast = res.value();
    RaychelScript::pretty_print_ast(ast);
}

[[maybe_unused]] static void echo_AST_from_stdin() noexcept
{
    using namespace std::string_view_literals;

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    Logger::log("Welcome to the interactive RaychelScript parser!\n"
                "Enter any valid expression and the AST will be echoed back to you.\n"
                "If you wish to exit this mode, type 'exit'\n"
                "Lines ending with a backslash '\\' will be parsed together.\n");
    std::string line{};

    for (;;) {
        line.clear();
        RaychelScript::IndentHandler::reset_indent();

        {
            std::string _line;
            for (;;) {
                std::cout << (line.empty() ? ">>"sv : "->"sv);
                std::getline(std::cin, _line);

                if (_line.back() != '\\') {
                    line += _line;
                    break;
                }
                line += _line;
                line.back() = '\n';
            }
        }

        if (line == "exit") {
            break;
        }

        const auto AST_or_error = RaychelScript::Parser::_parse_no_config_check(line);

        if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&AST_or_error); ec) {
            Logger::log("<ERROR>: ", *ec, '\n');
            continue;
        }

        const auto ast = Raychel::get<RaychelScript::AST>(AST_or_error);

        pretty_print_ast(ast);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
        echo_AST_from_stdin();
    else {
        std::ifstream input_stream{argv[1]}; //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const auto AST_or_error = RaychelScript::Parser::parse(input_stream);

        if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&AST_or_error); ec) {
            Logger::log("<ERROR>: ", *ec, '\n');
            return 1;
        }

        const auto ast = Raychel::get<RaychelScript::AST>(AST_or_error);

        pretty_print_ast(ast);
    }
    return 0;
}
