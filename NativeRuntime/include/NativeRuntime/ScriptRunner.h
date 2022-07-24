/**
* \file ScriptRunner.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ScriptRunner class
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
#ifndef RAYCHELSCRIPT_NATIVE_RUNTIME_SCRIPT_RUNNER_H
#define RAYCHELSCRIPT_NATIVE_RUNTIME_SCRIPT_RUNNER_H

#include "RuntimeErrorCode.h"

#include "RaychelCore/ClassMacros.h"
#include "RaychelCore/Raychel_assert.h"

#include <span>
#include <utility>

namespace RaychelScript::Runtime {

    class ScriptRunner
    {
        using EntryPoint = void (*)(double const* const input_vector, double* const output_vector) noexcept;

        template <std::uint32_t NumOutputs>
        struct Result
        {
            RuntimeErrorCode error_code{};
            std::array<double, NumOutputs> values{};
        };

    public:
        explicit ScriptRunner(std::string_view path_to_binary) noexcept
        {
            _try_initialize(path_to_binary);
        }

        RAYCHEL_MAKE_NONCOPY(ScriptRunner)
        RAYCHEL_MAKE_DEFAULT_MOVE(ScriptRunner)

        [[nodiscard]] RuntimeErrorCode get_initialization_status() const noexcept
        {
            return initialization_error_code_;
        }

        [[nodiscard]] bool initialized() const noexcept
        {
            return get_initialization_status() == RuntimeErrorCode::ok;
        }

        template <std::uint32_t NumOutputs, std::forward_iterator It>
        Result<NumOutputs> run(It begin, It end) const noexcept
        {
            return run<NumOutputs>(std::span{begin, end});
        }

        template <std::uint32_t NumOutputs>
        Result<NumOutputs> run(std::span<const double> inputs) const noexcept
        {
            if (!initialized()) {
                return {initialization_error_code_};
            }
            if (NumOutputs != script_output_vector_size_) {
                return {RuntimeErrorCode::mismatched_output_vector_size};
            }
            if (std::cmp_not_equal(inputs.size(), script_input_vector_size_)) {
                return {RuntimeErrorCode::mismatched_input_vector_size};
            }

            std::array<double, NumOutputs> outputs{};

            RAYCHEL_ASSERT(entry_point_ != nullptr);
            entry_point_(inputs.data(), outputs.data());

            return {.values = outputs};
        }

        ~ScriptRunner() noexcept
        {
            _destroy();
        }

    private:
        //These functions are platform dependent
        void _try_initialize(std::string_view path_to_binary) noexcept;
        void _destroy() noexcept;

        RuntimeErrorCode initialization_error_code_{RuntimeErrorCode::unit_not_initialized};
        EntryPoint entry_point_{};
        std::uint32_t script_input_vector_size_{};
        std::uint32_t script_output_vector_size_{};

        void* platform_specific_data_{};
    };

} // namespace RaychelScript::Runtime

#endif //!RAYCHELSCRIPT_NATIVE_RUNTIME_SCRIPT_RUNNER_H
