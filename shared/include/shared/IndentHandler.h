/**
* \file IndentHandler.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for IndentHandler class
* \date 2021-10-28
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
#ifndef RAYCHELSCRIPT_INDENT_HANDLER_H
#define RAYCHELSCRIPT_INDENT_HANDLER_H

#include <string_view>

#include <RaychelCore/ClassMacros.h>
#include <RaychelCore/Raychel_assert.h>

#ifdef _WIN32

    #ifdef RaychelScriptBase_EXPORTS
        #define RAYCHELSCRIPT_BASE_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_BASE_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_BASE_API
#endif

namespace RaychelScript {

    namespace details {

        class ThreadSafeIndentHandler
        {
        public:
            [[nodiscard]] static std::size_t increment() noexcept
            {
                return _instance().indent_++;
            }

            static std::size_t decrement() noexcept
            {
                return _instance().indent_--;
            }

            void static reset() noexcept
            {
                _instance().indent_ = 0U;
            }

        private:
            ThreadSafeIndentHandler() = default;

            [[nodiscard]] static ThreadSafeIndentHandler& _instance() noexcept
            {
                static thread_local ThreadSafeIndentHandler instance_{};
                return instance_;
            }

            std::size_t indent_{0};
        };

    } // namespace details

    /**
    * \brief Class for handling indent-based pretty printing in recursive call chains.
    * 
    * Every instance of this class increments the internal indent counter of the class on construction and decrements it on destruction.
    * 
    * \note this is a very idiomatic solution
    * 
    */
    class IndentHandler
    {
        static constexpr std::string_view indent_string{
            "...................................................................................................."};

    public:
        IndentHandler() : indent_{details::ThreadSafeIndentHandler::increment()}
        {
            RAYCHEL_ASSERT(indent_ != 50);
        }

        RAYCHEL_MAKE_NONCOPY_NONMOVE(IndentHandler)

        /**
        * \brief reset the internal indent counter
        * 
        */
        static void reset_indent() noexcept
        {
            details::ThreadSafeIndentHandler::reset();
        }

        /**
        * \brief Return an indent string for printing
        * 
        * \return An indent string based on the internal indent counter
        */
        [[nodiscard]] auto indent() const noexcept
        {
            //this is very very stupid
            return indent_string.substr(0, indent_ * 2U);
        }

        ~IndentHandler() noexcept
        {
            details::ThreadSafeIndentHandler::decrement();
        }

    private:
        std::size_t indent_;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_INDENT_HANDLER_H