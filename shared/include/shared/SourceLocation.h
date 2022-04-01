/**
* \file SourceLocation.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for SourceLocation struct
* \date 2021-10-03
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
#ifndef RAYCHELSCRIPT_SOURCE_LOCATION_H
#define RAYCHELSCRIPT_SOURCE_LOCATION_H

#include <iostream>

namespace RaychelScript {
    /**
    * \brief Class for representing locations inside a source file
    */
    struct SourceLocation
    {
        std::size_t line{0};
        std::size_t column{0};
    };

    inline std::ostream& operator<<(std::ostream& os, const SourceLocation& obj)
    {
        return os << '<' << obj.line << ":" << obj.column << '>';
    }
} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_SOURCE_LOCATION_H
