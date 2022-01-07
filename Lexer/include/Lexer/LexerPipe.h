/**
* \file LexerPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Lexer pipe API
* \date 2021-12-04
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
#ifndef RAYCHELSCRIPT_PIPES_LEXER_H
#define RAYCHELSCRIPT_PIPES_LEXER_H

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "Lexer.h"
#include "shared/Pipes/PipeResult.h"

namespace RaychelScript::Pipes {

    namespace details {
        struct LexFileTag
        {};
    }; // namespace details

    constexpr details::LexFileTag lex_file{};

    class Lex
    {
    public:
        explicit Lex(std::string_view source_text)
        {
            source_stream_ = std::make_unique<std::stringstream>(std::string{source_text});
        }

        Lex(details::LexFileTag /*unused*/, std::string_view file_path)
        {
            source_stream_ = std::make_unique<std::ifstream>(std::string{file_path});
        }

        Lexer::LexResult operator()() const noexcept
        {
            if (!source_stream_) {
                return Lexer::LexerErrorCode::no_input;
            }

            return Lexer::lex(*source_stream_);
        }

        operator PipeResult<Lexer::SourceTokens>() const noexcept //NOLINT: we want this conversion operator to be implicit
        {
            return (*this)();
        }

    private:
        std::unique_ptr<std::istream> source_stream_;
    };

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_PIPES_LEXER_H