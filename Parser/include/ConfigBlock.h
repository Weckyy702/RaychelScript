/**
* \file ConfigBlock.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ConfigBlock class
* \date 2021-10-01
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
#ifndef RAYCHELSCRIPT_CONFIG_BLOCK_H
#define RAYCHELSCRIPT_CONFIG_BLOCK_H

#include <map>
#include <string>
#include <vector>

namespace RaychelScript {

    struct ConfigBlock
    {
        std::vector<std::string> input_identifiers;
        std::vector<std::string> output_identifiers;
        std::map<std::string, std::vector<std::string>> config_vars;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_CONFIG_BLOCK_H