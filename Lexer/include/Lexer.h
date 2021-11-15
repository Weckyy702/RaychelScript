/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_LEXER_H
#define RAYCHELSCRIPT_LEXER_H

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
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

    /**
    * \brief Produce a 'raw' token stream. Use is discouraged
    * 
    * \param source_stream Input stream to read source code form
    * \return list of lexed tokens
    */
    RAYCHELSCRIPT_LEXER_API std::optional<std::vector<Token>> _lex_raw(std::istream& source_stream) noexcept;

    /**
    * \brief Turn a raw token stream into a list of lines
    * 
    * \param raw_tokens list of raw tokens
    * \return list of lines
    */
    RAYCHELSCRIPT_LEXER_API std::optional<std::vector<std::vector<Token>>>
    combine_tokens_into_lines(const std::optional<std::vector<Token>>& raw_tokens) noexcept;

    inline std::optional<std::vector<std::vector<Token>>> lex(std::istream& source_stream) noexcept
    {
        return combine_tokens_into_lines(_lex_raw(source_stream));
    }

    inline std::optional<std::vector<std::vector<Token>>> lex(const std::string& source_text) noexcept
    {
        std::stringstream stream{source_text};
        return lex(stream);
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_LEXER_H