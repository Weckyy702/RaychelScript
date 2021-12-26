/**
* \file WritePipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for WritePipe class
* \date 2021-12-25
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
#ifndef RAYCHELSCRIPT_WRITE_PIPE_H
#define RAYCHELSCRIPT_WRITE_PIPE_H

#include "VMData.h"
#include "write.h"

#include <fstream>
#include <string_view>

#include "shared/AST/AST.h"
#include "shared/Pipes/PipeResult.h"

namespace RaychelScript::Pipes {

    struct Write
    {

    public:
        explicit Write(std::string_view output_path) : output_stream_{std::string{output_path}, std::ios::binary}
        {}

        bool operator()(const Assembly::VMData& data) const noexcept
        {
            return Assembly::write_rsbf(output_stream_, data);
        }

    private:
        mutable std::ofstream output_stream_;
    };

    inline PipeResult<void> operator|(const PipeResult<Assembly::VMData>& input, const Write& writer) noexcept
    {
        RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(input);
        writer(input.value()); //TODO: handle errors
        return {};
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_WRITE_PI_H