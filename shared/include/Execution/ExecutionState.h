/**
* \file ExecutionState.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ExecutionState class
* \date 2021-11-16
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
#ifndef RAYCHELSCRIPT_EXECUTION_STATE_H
#define RAYCHELSCRIPT_EXECUTION_STATE_H

#include <vector>
#include <map>

#include "ConstantDescriptor.h"
#include "VariableDescriptor.h"

namespace RaychelScript {

    template <std::floating_point T>
    struct ExecutionState
    {
        std::vector<ConstantDescriptor<T>> input_vars;
        std::vector<VariableDescriptor<T>> output_vars;
        std::vector<VariableDescriptor<T>> runtime_vars;

        std::map<std::string_view, DescriptorID> descriptor_table;
    };

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_EXECUTION_STATE_H