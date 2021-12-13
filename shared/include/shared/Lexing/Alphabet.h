/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_ALPHABET_H
#define RAYCHELSCRIPT_ALPHABET_H

#include <array>
#include <cctype>

#include "shared/Lexing/TokenType.h"

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
        // clang-format on
    }

    [[nodiscard]] inline bool is_identifier_char(char c)
    {
        return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
    }

    [[nodiscard]] inline bool is_number_char(char c)
    {
        return std::isdigit(static_cast<unsigned char>(c)) != 0;
    }

    /**
    * \brief Check if an operator token falls into the MD part of PEMDAS
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] static bool is_MD_op(TokenType::TokenType type) noexcept
    {
        return (type == TokenType::star) || (type == TokenType::slash) || (type == TokenType::caret);
    }

    /**
    * \brief Check if an operator token falls into the AS part of PEMDAS
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] inline bool is_AS_op(TokenType::TokenType type) noexcept
    {
        return (type == TokenType::plus) || (type == TokenType::minus);
    }

    /**
    * \brief Check if a token is an arithmetic operator
    * 
    * \param token token to check
    * \return
    */
    [[nodiscard]] inline bool is_arith_op(TokenType::TokenType type) noexcept
    {
        return is_MD_op(type) || is_AS_op(type);
    }

    [[nodiscard]] inline bool is_allowed_token(TokenType::TokenType type) noexcept
    {
        return (type == TokenType::number) || (type == TokenType::identifer) || (type == TokenType::pipe) ||
               (type == TokenType::bang);
    }

    /**
    * \brief Check if a token is a closing parenthesis
    * 
    * \param type token to check
    * \return
    */
    [[nodiscard]] inline bool is_closing_parenthesis(TokenType::TokenType type) noexcept
    {
        return (type == TokenType::right_paren) || (type == TokenType::right_bracket) || (type == TokenType::right_curly);
    }

    /**
    * \brief Check if a token is an opening parenthesis
    * 
    * \param type token to check
    * \return
    */
    [[nodiscard]] inline bool is_opening_parenthesis(TokenType::TokenType type) noexcept
    {
        return (type == TokenType::left_paren) || (type == TokenType::left_bracket) || (type == TokenType::left_curly);
    }

    /**
    * \brief Check if a token is a parenthesis
    * 
    * \param type token to check
    * \return
    */
    [[nodiscard]] inline bool is_parenthesis(TokenType::TokenType type) noexcept
    {
        return is_opening_parenthesis(type) || is_closing_parenthesis(type);
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_ALPHABET_H