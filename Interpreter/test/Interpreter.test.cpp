#include "Interpreter.h"

#include "Parser.h"

#include <charconv>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "RaychelCore/AssertingGet.h"

int main()
{
    Logger::setMinimumLogLevel(Logger::LogLevel::error);

    std::vector<std::thread> threads;

    bool done{false};

    std::condition_variable stop_var;
    std::mutex mtx;

    std::uint64_t average_duration{0};
    constexpr std::size_t iterations = 1;

    for (std::size_t i = 0; i < iterations; i++) {
        /*threads.emplace_back(*/[&stop_var, &mtx, &done, &average_duration, i] {
            Logger::log("Thread number ", i + 1, '\n');

            //NOLINTBEGIN this is a very evil hacky solution
            char name[10] = {};
            std::to_chars(std::begin(name), std::end(name), i + 1);
            const auto label = Logger::startTimer({reinterpret_cast<char*>(name), sizeof(name)});
            //NOLINTEND

            const auto state_or_error_code = RaychelScript::Interpreter::interpret(
                R"source_code(
            [[config]]
            input a b
            output c

            [[body]]
            let d = a + b
            var d2 = 2 * d
            d2 *= d
            d2 *= 3.5
            c = d2

            #let fac = (a / 10)!
            )source_code",
                {{"a", i}, {"b", 1}});

            average_duration += Logger::getTimer<std::chrono::microseconds>(label).count();
            Logger::logDuration<std::chrono::microseconds>(Logger::LogLevel::log, label);

            /*std::unique_lock lck{mtx};

            stop_var.wait(lck, [&done] { return done; });*/

            if (const auto* ec = std::get_if<RaychelScript::Interpreter::InterpreterErrorCode>(&state_or_error_code); ec) {
                Logger::error(
                    "Error during execution! Reason: ", RaychelScript::Interpreter::error_code_to_reason_string(*ec), '\n');
            } else {
                const auto state = Raychel::get<RaychelScript::Interpreter::InterpreterState<double>>(state_or_error_code);

                Logger::info(
                    "SUCCESS from thread ", i + 1, ". c=", RaychelScript::get_identifier_value(state, "c").value_or(0.0), '\n');
                Logger::info(
                    "a=",
                    RaychelScript::get_identifier_value(state, "a").value_or(0.0),
                    " -> fac=",
                    RaychelScript::get_identifier_value(state, "fac").value_or(0.0),
                    '\n');

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
        }/*)*/();
    }

    done = true;
    stop_var.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    Logger::log("Average duration: ", average_duration / iterations, "us\n");
}