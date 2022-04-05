#include "NativeRuntime/ScriptRunner.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <chrono>

int main(int argc, char** argv){
    if(argc < 2) {
        return 1;
    }

    const std::string binary_name{argv[1]};
    auto input_vector = [&]() -> std::vector<double> {
        std::vector<double> res{};
        for(int i{2}; i != argc; ++i) {
            char* end{};
            if(const auto val = std::strtod(argv[i], &end); end != argv[i]) {
                res.push_back(val);
            }
        }
        return res;
    }();

    const RaychelScript::Runtime::ScriptRunner runner{binary_name};
    if(!runner.initialized()) {
        std::cout << "Initialization error: " << runner.get_initialization_status() << '\n';
        return 1;
    }

    const auto start = std::chrono::high_resolution_clock::now();

    const auto[error_code, values] = runner.run<1>(input_vector);

    const auto end = std::chrono::high_resolution_clock::now();

    if(error_code != RaychelScript::Runtime::RuntimeErrorCode::ok) {
        std::cout << "Runtime error: " << error_code << '\n';
        return 1;
    }

    for(const auto value : values) {
        std::cout << value << ", ";
    }

    std::cout << "runner.run took " << duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns\n";

    return 0;
}
