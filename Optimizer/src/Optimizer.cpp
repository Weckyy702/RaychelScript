/**
* \file Optimizer.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for optimization functions
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

#include "Optimizer/Optimizer.h"
#include "Optimizer/modules/AllModules.h"

#include <chrono>

namespace RaychelScript::Optimizer {

    [[nodiscard]] AST optimize(const AST& ast, const std::vector<OptimizerModule_p>& modules) noexcept
    {
        if (modules.empty()) {
            return ast;
        }

        AST new_ast = ast;

        for (const auto& module : modules) {
            const auto label = Logger::startTimer(module->name());
            (*module)(new_ast);
            Logger::logDuration<std::chrono::microseconds>(Logger::LogLevel::debug, label);
        }

        return new_ast;
    }

    [[nodiscard]] std::vector<OptimizerModule_p> get_optimization_modules_for_level(OptimizationLevel level) noexcept
    {
        std::vector<OptimizerModule_p> modules;
        switch (level) {
            case OptimizationLevel::none:
                break;
            case OptimizationLevel::light:
                modules.push_back(std::make_unique<RemoveIfNoSideEffects>());
                modules.push_back(std::make_unique<OptimizeConditionalsLight>());
                break;
            case OptimizationLevel::hard:
                modules.push_back(std::make_unique<RemoveIfNoSideEffects>());
                modules.push_back(std::make_unique<OptimizeConditionalsLight>());
                break;
            case OptimizationLevel::all:
                modules.push_back(std::make_unique<RemoveIfNoSideEffects>());
                modules.push_back(std::make_unique<OptimizeConditionalsLight>());
                break;
        }
        return modules;
    }

} //namespace RaychelScript::Optimizer
