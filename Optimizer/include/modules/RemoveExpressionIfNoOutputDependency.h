/**
* \file RemoveExpressionIfNoOutputDependency.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for RemoveExpressionIfNoOutputDependency class
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
#ifndef RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_OUTPUT_DEPENDENCY_H
#define RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_OUTPUT_DEPENDENCY_H

#include "OptimizerModule.h"

namespace RaychelScript::Optimizer {

    struct RemoveExpressionIfNoOutputDependency final : OptimizerModule
    {
        RemoveExpressionIfNoOutputDependency() = default;

        RAYCHEL_MAKE_NONCOPY_NONMOVE(RemoveExpressionIfNoOutputDependency)

        [[nodiscard]] std::string_view name() const noexcept override
        {
            return "Remove expressions if output values do not depend on them";
        }

        void operator()(AST& ast) const noexcept override
        {
            (void)ast;
        }

        ~RemoveExpressionIfNoOutputDependency() noexcept override = default;
    };

} //namespace RaychelScript::Optimizer

#endif //!RAYCHELSCRIPT_OPTIMIZER_REMOVE_IF_NO_OUTPUT_DEPENDENCY_H