#include "Interpreter.h"

#include "Parser.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <charconv>
#include "RaychelCore/AssertingGet.h"

int main()
{
    Logger::setMinimumLogLevel(Logger::LogLevel::info);

    std::vector<std::thread> threads;

    bool done{false};

    std::condition_variable stop_var;
    std::mutex mtx;

    for (std::size_t i = 0; i < 10; i++) {
        threads.emplace_back([&stop_var, &mtx, &done, i] {
            Logger::log("Thread number ", i + 1, '\n');

            char c[3] = {0, 0, 0};
            std::to_chars(std::begin(c), std::end(c), i);
            const auto label = Logger::startTimer({c, 3});

            const auto state_or_error_code = RaychelScript::Interpreter::interpret(
                R"source_code(
            [[config]]
            input a b
            output c

            [[body]]
            let d = a + b
            var d2 = 2 * |d|
            d2 *= d
            d2 *= 3.5
            c = d2

            let fac = 4!
            )source_code",
                {{"a", i}, {"b", 1}});

            Logger::logDuration(label);

            std::unique_lock lck{mtx};

            stop_var.wait(lck, [&done] { return done; });

            if (const auto* ec = std::get_if<RaychelScript::Interpreter::InterpreterErrorCode>(&state_or_error_code); ec) {
                Logger::error(
                    "Error during execution! Reason: ", RaychelScript::Interpreter::error_code_to_reason_string(*ec), '\n');
            } else {
                const auto state = Raychel::get<RaychelScript::ExecutionState<double>>(state_or_error_code);

                Logger::info("SUCCESS from thread ", i+1, ". c=", RaychelScript::get_identifier_value(state, "c").value_or(0.0), '\n');
                Logger::info("fac=", RaychelScript::get_identifier_value(state, "fac").value_or(0.0), '\n');

                // const auto state = Raychel::get<RaychelScript::ExecutionState<double>>(state_or_error_code);

                // Logger::log("Constant values: \n");
                // for (const auto& descriptor : state.constants) {
                //     Logger::log(
                //         '\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
                // }
                // Logger::log("Variable values: \n");
                // for (const auto& descriptor : state.variables) {
                //     Logger::log(
                //         '\t', RaychelScript::get_descriptor_identifier(state, descriptor.id()), ": ", descriptor.value(), '\n');
                // }
            }
        });
    }

    done = true;
    stop_var.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
}