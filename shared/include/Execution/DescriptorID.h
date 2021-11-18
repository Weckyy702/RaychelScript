/**
* \file DescriptorID.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for DescriptorID class
* \date 2021-11-16
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
#ifndef RAYCHELSCRIPT_DESCRIPTOR_ID_H
#define RAYCHELSCRIPT_DESCRIPTOR_ID_H

#include <compare>
#include <cstddef>
#include <ostream>
#include <atomic>

namespace RaychelScript {

    class DescriptorID
    {
        [[nodiscard]] static std::size_t _get_id() noexcept
        {
            static std::atomic_size_t s_id{0};
            return s_id++;
        }

    public:
        DescriptorID() : id_{_get_id()}
        {}

        [[nodiscard]] std::size_t id() const noexcept
        {
            return id_;
        }

        [[nodiscard]] std::size_t index() const noexcept
        {
            return id_ - id_base();
        }

        [[nodiscard]] static constexpr std::size_t id_base()
        {
            return 0xBEA115;
        }

        auto operator<=>(const DescriptorID& other) const noexcept = default;

    private:
        std::size_t id_{0};
    };

    inline std::ostream& operator<<(std::ostream& os, const DescriptorID& obj)
    {
        return os << obj.id();
    }

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_DESCRIPTOR_ID_H