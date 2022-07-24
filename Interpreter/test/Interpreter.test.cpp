/**
* \file Interpreter.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for Interpreter testing executable
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

#include "Interpreter/InterpreterPipe.h"
#include "Parser/ParserPipe.h"

#include "RaychelCore/AssertingGet.h"

#include <charconv>
#include <cstring>
#include <map>
#include <optional>

static std::optional<std::pair<std::string, double>>
try_parse_argument(char const* const* const argv, int argument_index) noexcept
{
    std::string argument_name{argv[argument_index]};
    const std::string_view arg_value_as_string{argv[argument_index + 1]};

    double value{};
    const auto [_, ec] =
        std::from_chars(arg_value_as_string.data(), arg_value_as_string.data() + arg_value_as_string.size(), value);

    if (ec != std::errc{}) {
        return std::nullopt;
    }

    return std::make_pair(std::move(argument_name), value);
}

int main(int argc, char** argv)
{
    using namespace RaychelScript::Pipes; //NOLINT(google-build-using-namespace)
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <script_file> <arg1_name> <arg1_value> ... <argN_name> <argN_value>\n";
        return 1;
    }

    if (argc % 2 != 0) {
        std::cout << "Expected even number of arguments!\n";
        return 1;
    }

    const auto args = [=] {
        std::map<std::string, double> res;

        for (int i = 2; i < argc; i += 2) {
            auto maybe_argument = try_parse_argument(argv, i);
            if (!maybe_argument.has_value()) {
                Logger::warn("Could not parse argument with name '", argv[i], "', value '", argv[i + 1], "'\n");
                continue;
            }
            res.insert(std::move(maybe_argument).value());
        }

        return res;
    }();

    const auto state_or_error = Lex{lex_file, argv[1]} | Parse{} | Interpret{args};

    if (log_if_error(state_or_error)) {
        return 1;
    }

    const auto state = state_or_error.value();

    for (const auto& scope : state.scopes) {
        for (const auto& [name, descriptor] : scope.descriptor_table) {
            if (descriptor.is_constant) {
                Logger::log("Constant ", name, " = ", state.constants.at(descriptor.index).value_or(0.0), '\n');
            } else {
                Logger::log("Variable ", name, " = ", state.variables.at(descriptor.index), '\n');
            }
        }
    }
}
