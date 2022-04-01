/**
* \file Lexer.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Lexer class
* \date 2022-02-05
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

#ifndef RAYCHELSCRIPT_LEXER_H
#define RAYCHELSCRIPT_LEXER_H

#include <fstream>
#include <sstream>
#include <string>

#include "LexResult.h"
#include "shared/Lexing/Token.h"

#ifdef _WIN32
    #ifdef RaychelScriptLexer_EXPORTS
        #define RAYCHELSCRIPT_LEXER_API __declspec(dllexport)
    #else
        #define RAYCHELSCRIPT_LEXER_API __declspec(dllimport)
    #endif
#else
    #define RAYCHELSCRIPT_LEXER_API
#endif

namespace RaychelScript::Lexer {

    [[nodiscard]] RAYCHELSCRIPT_LEXER_API LexResult lex(std::istream& source_stream) noexcept;

    [[nodiscard]] RAYCHELSCRIPT_LEXER_API SourceTokens lex_until_invalid_or_eof(std::istream& source_stream) noexcept;

} // namespace RaychelScript::Lexer

#endif //!RAYCHELSCRIPT_LEXER_H
