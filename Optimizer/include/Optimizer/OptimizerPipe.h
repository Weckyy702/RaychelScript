/**
* \file OptimizerPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Optimizer pipe API
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
#ifndef RAYCHELSCRIPT_PIPE_OPTIMIZER_H
#define RAYCHELSCRIPT_PIPE_OPTIMIZER_H

#include "Optimizer.h"
#include "shared/AST/AST.h"
#include "shared/Pipes/PipeResult.h"

#include "RaychelCore/AssertingGet.h"

namespace RaychelScript::Pipes {

    class Optimize
    {

    public:
        explicit Optimize(Optimizer::OptimizationLevel level) : modules_{Optimizer::get_optimization_modules_for_level(level)}
        {}

        explicit Optimize(std::vector<Optimizer::OptimizerModule_p> modules) : modules_{std::move(modules)}
        {}

        AST operator()(const AST& input) const noexcept
        {
            return Optimizer::optimize(input, modules_);
        }

    private:
        std::vector<Optimizer::OptimizerModule_p> modules_;
    };

    inline PipeResult<AST> operator|(const PipeResult<AST>& input, const Optimize& optimizer) noexcept
    {
        if (input.is_error()) {
            return input;
        }
        return optimizer(input.value());
    }

    //You can also use individual modules in the pipes API :)
    inline PipeResult<AST> operator|(const PipeResult<AST>& input, const Optimizer::OptimizerModule& module) noexcept
    {
        if (input.is_error()) {
            return input;
        }

        AST ast = input.value();
        module(ast);
        return ast;
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_PIPE_OPTIMIZER_H