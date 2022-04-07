/**
* \file Linux64.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for ScriptRunner class on 64-bit linux
* \date 2022-04-05
*
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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

#include "../include/NativeRuntime/ScriptRunner.h"

#include <dlfcn.h>

namespace RaychelScript::Runtime {

    void ScriptRunner::_try_initialize(std::string_view path_to_binary) noexcept
    {
        platform_specific_data_ = dlopen(std::string{path_to_binary}.data(), RTLD_LAZY);
        if (platform_specific_data_ == nullptr) {
            initialization_error_code_ = RuntimeErrorCode::binary_not_found;
            return;
        }

        entry_point_ = reinterpret_cast<EntryPoint>(dlsym(platform_specific_data_, "raychelscript_entry"));
        if (entry_point_ == nullptr) {
            initialization_error_code_ = RuntimeErrorCode::entry_point_not_found;
            return;
        }

        const auto* input_vector_len_ptr =
            reinterpret_cast<std::uint32_t*>(dlsym(platform_specific_data_, "raychelscript_input_vector_size"));
        if (input_vector_len_ptr == nullptr) {
            initialization_error_code_ = RuntimeErrorCode::input_vector_length_not_found;
            return;
        }
        script_input_vector_size_ = *input_vector_len_ptr;

        const auto* output_vector_len_ptr =
            reinterpret_cast<std::uint32_t*>(dlsym(platform_specific_data_, "raychelscript_output_vector_size"));
        if (output_vector_len_ptr == nullptr) {
            initialization_error_code_ = RuntimeErrorCode::output_vector_length_not_found;
            return;
        }
        script_output_vector_size_ = *output_vector_len_ptr;

        initialization_error_code_ = RuntimeErrorCode::ok;
    }

    void ScriptRunner::_destroy() noexcept
    {
        if (platform_specific_data_ == nullptr) {
            return;
        }
        RAYCHEL_ASSERT(dlclose(platform_specific_data_) == 0);
    }

} //namespace RaychelScript::Runtime
