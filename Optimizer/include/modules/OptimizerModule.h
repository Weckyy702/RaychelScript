/**
* \file OptimizerModule.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for OptimizerModule class
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
#ifndef RAYCHELSCRIPT_OPTIMIZER_MODULE_H
#define RAYCHELSCRIPT_OPTIMIZER_MODULE_H

#include <memory>
#include <string_view>

#include "RaychelCore/ClassMacros.h"

namespace RaychelScript {
    struct AST;
} //namespace RaychelScript

namespace RaychelScript::Optimizer {

    struct OptimizerModule
    {

        OptimizerModule() = default;

        RAYCHEL_MAKE_NONCOPY_NONMOVE(OptimizerModule)

        [[nodiscard]] virtual std::string_view name() const noexcept = 0;

        virtual void operator()(AST& ast) const noexcept = 0; //TODO: can optimization passes fail? If so, return some error code

        virtual ~OptimizerModule() = default;
    };

    using OptimizerModule_p = std::unique_ptr<OptimizerModule>;

} //namespace RaychelScript::Optimizer

#endif //!RAYCHELSCRIPT_OPTIMIZER_MODULE_H