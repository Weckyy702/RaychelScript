/**
* \file write.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Implementation file for write class
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

#include "rasm/write.h"

#include <array>
#include <bit>
#include <concepts>
#include <cstring>
#include <limits>

#include "RaychelCore/Raychel_assert.h"

#define TRY(expression)                                                                                                          \
    if (!(expression)) {                                                                                                         \
        return false;                                                                                                            \
    }

namespace RaychelScript::Assembly {

    template <typename T>
    requires std::is_trivial_v<T>
    bool write(std::ostream& stream, T data) noexcept
    {
        constexpr auto byte_size = sizeof(T);
        std::array<char, byte_size> bytes{};

        std::memcpy(bytes.data(), &data, byte_size); //non-UB reinterpret_cast

        stream.write(bytes.data(), byte_size);

        return stream.good();
    }

    bool write(std::ostream& stream, const std::string& str) noexcept
    {
        RAYCHEL_ASSERT(str.size() < std::numeric_limits<std::uint32_t>::max());
        TRY(write(stream, static_cast<std::uint32_t>(str.size())))
        for (char elem : str) {
            TRY(write(stream, elem))
        }
        return true;
    }

    bool write(std::ostream& stream, MemoryIndex index) noexcept
    {
        return write(stream, index.value());
    }

    template <typename T1, typename T2>
    bool write(std::ostream& stream, const std::pair<T1, T2>& pair) noexcept
    {
        TRY(write(stream, pair.first));
        TRY(write(stream, pair.second));
        return true;
    }

    template <typename T>
    bool write(std::ostream& stream, const std::vector<T>& vec) noexcept
    {
        RAYCHEL_ASSERT(vec.size() < std::numeric_limits<std::uint32_t>::max());
        TRY(write(stream, static_cast<std::uint32_t>(vec.size())))
        for (const auto& elem : vec) {
            TRY(write(stream, elem))
        }
        return true;
    }

    [[nodiscard]] bool write_rsbf(std::ostream& stream, const VM::VMData& data) noexcept
    {
        if (!stream) {
            return false;
        }

        //magic word / version number
        TRY(write(stream, magic_word))
        TRY(write(stream, version_number()))

        //Number of memory locations
        TRY(write(stream, data.num_memory_locations));

        //I/O section
        TRY(write(stream, data.config_block.input_identifiers))
        TRY(write(stream, data.config_block.output_identifiers))

        //Immediate section
        TRY(write(stream, data.immediate_values));

        //Text section
        RAYCHEL_ASSERT(data.instructions.size() < std::numeric_limits<std::uint32_t>::max());
        TRY(write(stream, static_cast<std::uint32_t>(data.instructions.size())))
        for (const auto& instr : data.instructions) {
            TRY(write(stream, instr.to_binary()))
        }

        return true;
    }

} //namespace RaychelScript::Assembly

#undef TRY
