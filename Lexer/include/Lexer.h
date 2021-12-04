/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_LEXER_H
#define RAYCHELSCRIPT_LEXER_H

#include <fstream>
#include <sstream>
#include <string>

#include "LexResult.h"
#include "Lexing/Token.h"

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

} // namespace RaychelScript::Lexer

#endif //!RAYCHELSCRIPT_LEXER_H