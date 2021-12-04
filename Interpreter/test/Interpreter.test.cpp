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

#include "Interpreter.h"

#include "Parser.h"

#include <charconv>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "RaychelCore/AssertingGet.h"

int main()
{
    Logger::setMinimumLogLevel(Logger::LogLevel::debug);

    std::vector<std::thread> threads;

    bool done{false};

    std::condition_variable stop_var;
    std::mutex mtx;

    std::uint64_t average_duration{0};
    constexpr std::size_t iterations = 100;

    for (std::size_t i = 0; i < iterations; i++) {
        threads.emplace_back([&stop_var, &mtx, &done, &average_duration, i] {
            Logger::log("Thread number ", i + 1, '\n');

            //NOLINTBEGIN this is a very evil hacky solution
            char name[10] = {};
            std::to_chars(std::begin(name), std::end(name), i + 1);
            const auto label = Logger::startTimer({reinterpret_cast<char*>(name), sizeof(name)});
            //NOLINTEND

            std::ifstream file{"../../../shared/test/conditionals.rsc"};
            const auto state_or_error_code = RaychelScript::Interpreter::interpret(file, {{"_a", -1}, {"b", 1}});

            average_duration += Logger::getTimer<std::chrono::microseconds>(label).count();
            Logger::logDuration<std::chrono::microseconds>(Logger::LogLevel::log, label);

            std::unique_lock lck{mtx};

            stop_var.wait(lck, [&done] { return done; });

            if (const auto* ec = std::get_if<RaychelScript::Interpreter::InterpreterErrorCode>(&state_or_error_code); ec) {
                Logger::error(
                    "Error during execution! Reason: ", RaychelScript::Interpreter::error_code_to_reason_string(*ec), '\n');
            } else {
                const auto state = Raychel::get<RaychelScript::Interpreter::InterpreterState<double>>(state_or_error_code);

                Logger::info(
                    "SUCCESS from thread ", i + 1, ". c=", RaychelScript::get_identifier_value(state, "c").value_or(0.0), '\n');

                Logger::log("Constant values: \n");
                for (const auto& descriptor : state.constants) {
                    Logger::log(
                        '\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
                }
                Logger::log("Variable values: \n");
                for (const auto& descriptor : state.variables) {
                    Logger::log(
                        '\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
                }
            }
        });
    }

    done = true;
    stop_var.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    Logger::log("Average duration: ", average_duration / iterations, "us\n");
}