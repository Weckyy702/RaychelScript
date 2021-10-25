/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_LEXER_H
#define RAYCHELSCRIPT_LEXER_H

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "Token.h"

namespace RaychelScript {

    /**
    * \brief Produce a 'raw' token stream. This means that -12 will become MINUS NUMBER. Use is discouraged
    * 
    * \param source_stream Input stream to read source code form
    * \return list of lexed tokens
    */
    std::optional<std::vector<Token>> lex_raw(std::istream& source_stream) noexcept;

    /**
    * \brief Turn a raw token stream into a list of lines
    * 
    * \param raw_tokens list of raw tokens
    * \return list of lines
    */
    std::optional<std::vector<std::vector<Token>>>
    combine_tokens_into_lines(const std::optional<std::vector<Token>>& raw_tokens) noexcept;

    inline std::optional<std::vector<std::vector<Token>>> lex(std::istream& source_stream) noexcept
    {
        return combine_tokens_into_lines(lex_raw(source_stream));
    }

    inline std::optional<std::vector<std::vector<Token>>> lex(const std::string& source_text) noexcept
    {
        std::stringstream s{source_text};
        return lex(s);
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_LEXER_H