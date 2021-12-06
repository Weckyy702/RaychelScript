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
#include "ParserPipe.h"

#include <RaychelCore/AssertingGet.h>
#include <fstream>
#include <string_view>
#include <vector>

#include "AST/NodeData.h"
#include "IndentHandler.h"
#include "Misc/PrintAST.h"

[[maybe_unused]] static void parse_file_and_print_debug_info(const std::string& filename) noexcept
{
    Logger::info("Parsing file '", filename, "'\n");
    std::ifstream in_file{};
    const auto file_path = "../../../shared/test/" + filename;

    const auto label = Logger::startTimer("parse time");


    const auto res = RaychelScript::Pipes::Lex(RaychelScript::Pipes::lex_file, file_path) | RaychelScript::Pipes::Parse{};


    Logger::logDuration<std::chrono::microseconds>(label);

    if (const RaychelScript::AST* ast = std::get_if<RaychelScript::AST>(&res); ast) {
        print_config_block(ast->config_block);
        pretty_print_ast(ast->nodes);
    } else {
        Logger::log(
            "The following error occured during parsing: ",
            error_code_to_reason_string(Raychel::get<RaychelScript::Parser::ParserErrorCode>(res)),
            '\n');
    }
}

[[maybe_unused]] static void echo_AST_from_stdin() noexcept
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    Logger::log(
        R"(Welcome to the interactive RaychelScript parser!
Enter any valid expression and the AST will be echoed back to you.
If you wish to exit this mode, type "exit")",
        '\n');
    std::string line;

    do {
        RaychelScript::IndentHandler::reset_indent();

        std::cout << ">>";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        const auto AST_or_error = RaychelScript::Parser::_parse_no_config_check(line);

        if (const auto* ec = std::get_if<RaychelScript::Parser::ParserErrorCode>(&AST_or_error); ec) {
            Logger::log("<ERROR>: ", error_code_to_reason_string(*ec), '\n');
            continue;
        }

        const auto ast = Raychel::get<RaychelScript::AST>(AST_or_error);

        pretty_print_ast(ast.nodes);

    } while (true);
}

int main()
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    parse_file_and_print_debug_info("conditionals.rsc");

    return 0;
}