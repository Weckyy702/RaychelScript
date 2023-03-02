/**
* \file VMState.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for VMState class
* \date 2022-06-25
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

#include "VM/VMState.h"

#include <span>

namespace RaychelScript::VM {

    //defined in VM.cpp
    void push_frame(VMState&, const CallFrameDescriptor&);

    VMState::VMState(details::Range<StackPointer> memory, details::Range<FramePointer> stack, const VMData& _data) noexcept
        : frame_pointer{stack.begin},
          stack_pointer{memory.begin},
          beginning_of_stack{stack.begin},
          end_of_stack{stack.end},
          end_of_memory{memory.end},
          data{_data}
    {
        const auto& global_frame = data.call_frames.front();
        new (std::to_address(frame_pointer)) CallFrame{global_frame.instructions.begin(), global_frame.size};
    }

} //namespace RaychelScript::VM
