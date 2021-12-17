/**
* \file read.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for functions related to reading instructions from RSBF files
* \date 2021-12-17
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_READ_H
#define RAYCHELSCRIPT_ASSEMBLY_READ_H

#include "VMData.h"
#include "magic.h"

#include <variant>
#include <istream>
#include <fstream>
#include <string>
#include <string_view>

namespace RaychelScript::Assembly {

    enum class ReadingErrorCode {
        ok,
        file_not_found,
        no_magic_word,
        wrong_version,
        reading_failure,
    };

    std::variant<VMData, ReadingErrorCode> read_rsbf(std::istream& stream) noexcept;

    inline std::variant<VMData, ReadingErrorCode> read_rsbf(std::string_view path) noexcept
    {
        std::ifstream stream{std::string{path}, std::ios::in | std::ios::binary};
        return read_rsbf(stream);
    }

}//namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_READ  _H