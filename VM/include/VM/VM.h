/**
* \file VM.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VM class
* \date 2021-12-27
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
#ifndef RAYCHELSCRIPT_VM_H
#define RAYCHELSCRIPT_VM_H

#include "VMErrorCode.h"
#include "VMState.h"

#include "shared/VM/VMData.h"

#include <variant>
#include <vector>

#ifdef _WIN32
    #ifdef RaychelScriptVM_EXPORTS
        #define RAYCHELSCRIPT_VM_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_VM_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_VM_API
#endif

namespace RaychelScript::VM {

    template <std::floating_point T>
    using VMResult = std::variant<VMErrorCode, VMState<T>>;

    template <std::floating_point T>
    RAYCHELSCRIPT_VM_API [[nodiscard]] VMResult<T> execute(const VMData& data, const std::vector<T>& input_variables) noexcept;

    namespace details {
        inline void _instantiate_entry_points_header() noexcept
        {
            (void)execute<float>({}, {});
            (void)execute<double>({}, {});
            (void)execute<long double>({}, {});
        }
    }; //namespace details
} //namespace RaychelScript::VM

#endif //!RAYCHELSCRIPT_VM_H