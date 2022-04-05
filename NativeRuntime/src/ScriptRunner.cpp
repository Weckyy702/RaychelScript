/**
* \file ScriptRunner.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for ScriptRunner class
* \date 2022-04-05
*
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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
#include "RaychelCore/compat.h"
#include <cstdint>

#if RAYCHEL_ACTIVE_OS == RAYCHEL_OS_LINUX
    #if INTPTR_MAX == INT64_MAX
        #include "Linux64.cpp"
    #elif INTPTR_MAX == INT32_MAX
        #error "32-bit linux is not (yet) supported!"
    #else
        #error "Unrecognized linux architecture!"
    #endif
#else
    #error "Non-linux OS is not (yet) supported!"
#endif
