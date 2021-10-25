/**
* \file TokenType.hpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief TokenyType enum declaration
* \date 2021-09-28
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
#ifndef RAYCHELSCRIPT_TOKEN_TYPE_H
#define RAYCHELSCRIPT_TOKEN_TYPE_H

namespace RaychelScript::TokenType {

    enum TokenType : char {
        newline = '\n',
        left_paren = '(',
        right_paren = ')',
        left_bracket = '[',
        right_bracket = ']',
        left_curly = '{',
        right_curly = '}',
        comma = ',',
        number = '0',
        identifer = '1',
        declaration = '2',
        plus = '+',
        minus = '-',
        star = '*',
        slash = '/',
        percent = '%',
        equal = '=',
        left_angle = '<',
        right_angle = '>',
        bang = '!',
        ampersand = '&',
        pipe = '|',
        caret = '^',

        expression_ = '\0',
    };

} //namespace RaychelScript::TokenType

#endif //!RAYCHELSCRIPT_TOKEN_TYPE_H