/**
* \file RuntimeErrorCode.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for RuntimeErrorCode class
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
#ifndef RAYCHELSCRIPT_NATIVE_RUNTIME_ERROR_CODE_H
#define RAYCHELSCRIPT_NATIVE_RUNTIME_ERROR_CODE_H

#include <ostream>
#include <string_view>

namespace RaychelScript::Runtime {

    enum class RuntimeErrorCode : std::uint32_t {
        ok = 0U,
        mismatched_input_vector_size,
        mismatched_output_vector_size,
        unit_not_initialized,
        binary_not_found,
        entry_point_not_found,
        input_vector_length_not_found,
        output_vector_length_not_found,
    };

    constexpr std::string_view error_code_to_reason_string(RuntimeErrorCode ec)
    {
        switch (ec) {
            case RuntimeErrorCode::ok:
                return "Everything's fine :)";
            case RuntimeErrorCode::mismatched_input_vector_size:
                return "Wrong number of input identifiers was provided!";
            case RuntimeErrorCode::mismatched_output_vector_size:
                return "Output vector has wrong size!";
            case RuntimeErrorCode::unit_not_initialized:
                return "Script unit was not properly initialized!";
            case RuntimeErrorCode::binary_not_found:
                return "Script binary not found!";
            case RuntimeErrorCode::entry_point_not_found:
                return "Entry point not found in script binary!";
            case RuntimeErrorCode::input_vector_length_not_found:
                return "Input vector length not found in script binary!";
            case RuntimeErrorCode::output_vector_length_not_found:
                return "Output vector length not found in script binary!";
        }
        return "Unknown error code!";
    }

    inline std::ostream& operator<<(std::ostream& os, RuntimeErrorCode ec)
    {
        return os << error_code_to_reason_string(ec);
    }

} // namespace RaychelScript::Runtime

#endif //!RAYCHELSCRIPT_NATIVE_RUNTIME_ERROR_CODE_H
