/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_ALPHABET_H
#define RAYCHELSCRIPT_ALPHABET_H

#include <algorithm>
#include <array>
#include "TokenType.h"

namespace RaychelScript {

    [[nodiscard]] constexpr bool is_special_char(char c)
    {
        // clang-format off
        return  (c == TokenType::left_paren) || 
                (c == TokenType::right_paren) || 
                (c == TokenType::left_bracket) ||
                (c == TokenType::right_bracket) || 
                (c == TokenType::left_curly) || 
                (c == TokenType::right_curly) ||
                (c == TokenType::comma) || 
                (c == TokenType::plus) || 
                (c == TokenType::minus) || 
                (c == TokenType::star) ||
                (c == TokenType::slash) || 
                (c == TokenType::percent) || 
                (c == TokenType::equal) || 
                (c == TokenType::left_angle) ||
                (c == TokenType::right_angle) || 
                (c == TokenType::bang) || 
                (c == TokenType::ampersand) ||
                (c == TokenType::pipe) || 
                (c == TokenType::caret);
        // clang-format off
    }

    [[nodiscard]] constexpr bool is_identifier_char(char c)
    {
        return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
    }

    [[nodiscard]] constexpr bool is_number_char(char c)
    {
        return std::isdigit(static_cast<unsigned char>(c)) != 0;
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_ALPHABET_H