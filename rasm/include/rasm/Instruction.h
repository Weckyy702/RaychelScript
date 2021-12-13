/**
* \file Instruction.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Instruction class
* \date 2021-12-13
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
#ifndef RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H
#define RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H

#include "OpCode.h"

namespace RaychelScript::Assembly {

    class Instruction
    {
        //this struct exclusively exists to make clang-tidy happy
        struct IOData
        {
            explicit IOData(std::uint8_t);

            std::uint8_t input{0}, output{0};
        };

    public:
        explicit Instruction(OpCode code, IOData io_data) : code_{code}, input_{io_data.input}, output_{io_data.output}
        {}

        std::uint32_t to_binary() noexcept
        {
            /*
            Instruction layout:
            |........|........|........|........|
            OpCode    input    output   reserved
            */

            std::uint32_t instr = 0U;

            instr |= static_cast<std::uint32_t>(code_) << 24U;
            instr |= static_cast<std::uint32_t>(input_) << 16U;
            instr |= static_cast<std::uint32_t>(output_) << 8U;

            return instr;
        }

    private:
        OpCode code_;
        std::uint8_t input_;
        std::uint8_t output_;
    };

} //namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_ASSEMBLY_INSTRUCTION_H