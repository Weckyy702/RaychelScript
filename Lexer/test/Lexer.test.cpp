/**
* \file Lexer.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Main file for Lexer testing executable
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

#include "Lexer/Lexer.h"

#include <fstream>

#include "RaychelCore/AssertingGet.h"
#include "RaychelLogger/Logger.h"

int main(int /*unused*/, char** /*unused*/)
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    std::ifstream in_file{"../../../shared/test/conditionals.rsc"};
    const auto lines_or_error = RaychelScript::Lexer::lex(in_file);

    if (const auto* ec = std::get_if<RaychelScript::Lexer::LexerErrorCode>(&lines_or_error); ec) {
        return 1;
    }

    std::size_t line_no = 1U;

    for (const auto& line : Raychel::get<RaychelScript::Lexer::SourceTokens>(lines_or_error)) {
        Logger::log(line_no, ": ");
        for (const auto& [type, _1, _2] : line) {
            Logger::log(RaychelScript::token_type_to_string(type), ' ');
        }
        Logger::log('\n');
        line_no++;
    }
}