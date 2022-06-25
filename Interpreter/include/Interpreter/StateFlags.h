/**
* \file StateFlags.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for StateFlags enum
* \date 2021-12-07
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
#ifndef RAYCHELSCRIPT_OPTIMIZER_STATE_FLAGS_H
#define RAYCHELSCRIPT_OPTIMIZER_STATE_FLAGS_H

#include <cstdint>
#include <type_traits>

namespace RaychelScript::Interpreter {
    enum class StateFlags : std::uint32_t {
        none = 0x0U,
        zero = 0x1U,
        negative = 0x2U,
        condition_was_true = 0x4U,
        return_from_function = 0x8U,
        a = 0x10U,
    };

    inline StateFlags& operator|=(StateFlags& lhs, StateFlags rhs) noexcept
    {
        using T = std::underlying_type_t<StateFlags>;

        lhs = StateFlags{static_cast<T>(lhs) | static_cast<T>(rhs)};
        return lhs;
    }

    inline bool operator&(StateFlags lhs, StateFlags rhs) noexcept
    {
        using T = std::underlying_type_t<StateFlags>;
        return (static_cast<T>(lhs) & static_cast<T>(rhs)) != 0U;
    }

    inline bool operator!(StateFlags flags) noexcept
    {
        return flags == StateFlags::none;
    }
} //namespace RaychelScript::Interpreter

#endif //!RAYCHELSCRIPT_OPTIMIZER_STATE_FLAGS_H
