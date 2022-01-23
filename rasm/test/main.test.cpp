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

    Instruction mov{OpCode::mov, 12_mi, 42_mi};
    Instruction add{OpCode::add};
    Instruction sub{OpCode::sub};
    Instruction mul{OpCode::mul};
    Instruction div{OpCode::div};
    Instruction mag{OpCode::mag};
    Instruction fac{OpCode::fac};

    std::vector<Instruction> instructions{mov, add, div, Instruction{OpCode::mov, 0_mi, 12_mi}, sub, add, mov};

    if (!write_rsbf(
            "./instr.rsbf",
            RaychelScript::VM::VMData{
                .config_block = {{{"a", 1_mi}, {"b", 2_mi}}, {{"c", 3_mi}}},
                .immediate_values = {{0.1, 12_mi}, {12, 0_mi}, {99, 9_mi}},
                .instructions = instructions,
                .num_memory_locations = 12})) {
        Logger::error("Writing failed!\n");
        return 1;
    }

    const auto data_or_error = read_rsbf("./instr.rsbf");
    if (const auto* ec = std::get_if<ReadingErrorCode>(&data_or_error); ec) {
        Logger::error("Reading failed!\n");
        return 1;
    }

    Logger::info("Original\n");

    for (const auto instr : instructions) {
        Logger::info('\t', instr, '\n');
    }

    Logger::info("File\n");

    const auto data = Raychel::get<RaychelScript::VM::VMData>(data_or_error);

    for (const auto& [value, address] : data.config_block.input_identifiers) {
        Logger::info('\t', address, " -> ", value, '\n');
    }

    for (const auto& [value, address] : data.config_block.output_identifiers) {
        Logger::info('\t', address, " -> ", value, '\n');
    }

    for (const auto& [value, address] : data.immediate_values) {
        Logger::info('\t', address, " -> ", value, '\n');
    }

    for (const auto& instr : data.instructions) {
        Logger::info('\t', instr, '\n');
    }

    Logger::info(data.num_memory_locations, '\n');

    return 0;
}