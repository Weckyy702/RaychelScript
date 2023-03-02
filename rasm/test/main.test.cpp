/**
* \file main.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Main testing file for RaychelScript assembly library
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

#include "rasm/WritePipe.h"
#include "rasm/read.h"
#include "rasm/write.h"
#include "shared/rasm/Instruction.h"

#include <iostream>
#include <vector>

#include "RaychelCore/AssertingGet.h"

int main()
{
    using namespace RaychelScript::Assembly; //NOLINT
    using namespace RaychelScript::VM;       //NOLINT

    Instruction mov{OpCode::mov, 12_imm, 42_mi};
    Instruction add{OpCode::add, 0_imm, 4_mi};
    Instruction sub{OpCode::sub, 0_mi, 13_mi};
    Instruction mul{OpCode::mul, 0_mi, 1_mi};
    Instruction div{OpCode::div, 0_mi, 2_imm};
    Instruction mag{OpCode::mag, 0_mi};
    Instruction fac{OpCode::fac, 0_mi};

    const std::vector<Instruction> instructions{
        mov, add, div, Instruction{OpCode::mov, 0_imm, 12_mi}, sub, add, mov, Instruction{OpCode::hlt}};

    if (!write_rsbf(
            "./instr.rsbf",
            VMData{
                .num_input_identifiers = 3U,
                .num_output_identifiers = 1U,
                .immediate_values = {0.1, 12, 99},
                .call_frames = {
                    CallFrameDescriptor{.size = 16U, .instructions = instructions},
                    CallFrameDescriptor{
                        .size = 1U, .instructions = {Instruction{OpCode::mov, 3_imm, 0_mi}, Instruction{OpCode::ret}}}}})) {
        Logger::error("Writing failed!\n");
        return 1;
    }

    Logger::info("Original\n");

    for (const auto instr : instructions) {
        Logger::info('\t', instr, '\n');
    }

    const auto data_or_error = read_rsbf("./instr.rsbf");
    if (const auto* ec = std::get_if<ReadingErrorCode>(&data_or_error); ec) {
        Logger::error("Reading failed!\n");
        return 1;
    }

    Logger::info("File\n");

    const auto data = Raychel::get<VMData>(data_or_error);

    Logger::info(static_cast<std::uint32_t>(data.num_input_identifiers), " input constants\n");
    Logger::info(static_cast<std::uint32_t>(data.num_output_identifiers), " output variables\n");

    std::size_t i{};
    for (const auto& value : data.immediate_values) {
        Logger::info('%', i++, " -> ", value, '\n');
    }

    i = 0;
    for (const auto& frame : data.call_frames) {
        Logger::info("Call frame #", i++, " with memory size ", static_cast<std::uint32_t>(frame.size), ":\n");
        for (const auto& instr : frame.instructions) {
            Logger::info('\t', instr, '\n');
        }
    }

    return 0;
}
