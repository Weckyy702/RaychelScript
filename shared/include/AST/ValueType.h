/**
* \file ValueType.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ValueType enum
* \date 2021-11-30
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
#ifndef RAYCHELSCRIPT_VALUE_TYPE_H
#define RAYCHELSCRIPT_VALUE_TYPE_H

#include <string_view>
#include <ostream>

namespace RaychelScript {

    /**
    * \brief Enum for representing expression types
    */
    enum class ValueType { none, boolean, number };

    [[nodiscard]] inline std::string_view value_type_to_string(ValueType type) noexcept
    {
        using namespace std::string_view_literals;

        switch (type) {
            case ValueType::none:
                return "none"sv;
            case ValueType::boolean:
                return "boolean"sv;
            case ValueType::number:
                return "number"sv;
        }
        return "<unknown type>"sv;
    }

    inline std::ostream& operator<<(std::ostream& os, ValueType obj) noexcept
    {
        return os << value_type_to_string(obj);
    }

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_VALUE_TYPE_H