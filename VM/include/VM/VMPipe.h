/**
* \file VMPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VMPipe class
* \date 2021-12-29
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
#ifndef RAYCHELSCRIPT_VM_PIPE_H
#define RAYCHELSCRIPT_VM_PIPE_H

#include "VM.h"
#include "shared/Pipes/PipeResult.h"

#include <concepts>
#include <utility>

namespace RaychelScript::Pipes {

    template <std::floating_point T>
    class Execute
    {

    public:
        explicit Execute(std::vector<T> args) : args_{std::move(args)}
        {}

        VM::VMResult<T> operator()(const VM::VMData& data) const noexcept
        {
            return VM::execute(data, args_);
        }

    private:
        std::vector<T> args_;
    };

    template <std::floating_point T>
    PipeResult<std::pair<VM::VMData, VM::VMState<T>>>
    operator|(const PipeResult<VM::VMData>& input, const Execute<T>& vm) noexcept
    {
        RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(input);
        const auto state_or_error = vm(input.value());
        if (const auto* ec = std::get_if<VM::VMErrorCode>(&state_or_error); ec) {
            return *ec;
        }
        return std::make_pair(input.value(), Raychel::get<VM::VMState<T>>(state_or_error));
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_VM_PIPE_H
