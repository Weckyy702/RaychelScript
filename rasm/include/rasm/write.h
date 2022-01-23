/**
* \file write.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for functions related to writing instructions into RSBF files
* \date 2021-12-16
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_WRITE_H
#define RAYCHELSCRIPT_ASSEMBLY_WRITE_H

#include "magic.h"
#include "shared/VM/VMData.h"

#include <fstream>
#include <ostream>
#include <string>
#include <string_view>

namespace RaychelScript::Assembly {

    RAYCHELSCRIPT_ASSEMBLY_API [[nodiscard]] bool write_rsbf(std::ostream& stream, const VM::VMData& data) noexcept;

    [[nodiscard]] inline bool write_rsbf(std::string_view path, const VM::VMData& data) noexcept
    {
        std::ofstream stream{std::string{path}, std::ios::out | std::ios::binary};
        return write_rsbf(stream, data);
    }

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_WRITE_H