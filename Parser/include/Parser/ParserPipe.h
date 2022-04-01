/**
* \file ParserPipe.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Parser pipe API
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
#ifndef RAYCHELSCRIPT_PARSER_PIP_H
#define RAYCHELSCRIPT_PARSER_PIP_H

#include "Lexer/LexerPipe.h"
#include "Parser/Parser.h"
#include "shared/Pipes/PipeResult.h"

#include "RaychelCore/AssertingGet.h"

namespace RaychelScript::Pipes {

    struct Parse
    {
        Parser::ParseResult operator()(const Lexer::SourceTokens& input) const noexcept
        {
            return Parser::parse(input);
        }
    };

    inline PipeResult<AST> operator|(const PipeResult<Lexer::SourceTokens>& input, const Parse& parser) noexcept
    {
        RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(input);
        return parser(input.value());
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_PARSER_PIP_H
