/**
* \file VMData.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for VMData class
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
#ifndef RAYCHELSCRIPT_VM_DATA_H
#define RAYCHELSCRIPT_VM_DATA_H

#include "Instruction.h"
#include "shared/AST/ConfigBlock.h"

#include <vector>
namespace RaychelScript::Assembly {

    struct VMConfigBlock
    {
        using IdentiferContainer = std::vector<std::pair<std::string, MemoryIndex>>;
        
        IdentiferContainer input_identifiers;
        IdentiferContainer output_identifiers;
    };

    /**
    * \brief The VM equivalent of the AST struct for the interpreter
    */
    struct VMData
    {
        VMConfigBlock config_block;

        std::vector<std::pair<double, MemoryIndex>> immediate_values;
        //TODO: add information about the variables

        std::vector<Instruction> instructions;
        std::size_t num_memory_locations;
    };

} // namespace RaychelScript::Assembly

#endif //!RAYCHELSCRIPT_VM_DATA_H