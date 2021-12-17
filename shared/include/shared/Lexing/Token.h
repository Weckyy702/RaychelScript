/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_TOKEN_H
#define RAYCHELSCRIPT_TOKEN_H

#include <RaychelCore/AssertingOptional.h>
#include <string>

#include "shared/SourceLocation.h"
#include "TokenType.h"

namespace RaychelScript {

    /**
    * \brief Class for representing lexed tokens
    * 
    * This class holds a type and a location as well as an optional string for Tokens which require additional information (e.g identifiers).
    * 
    */
    struct Token
    {
        TokenType::TokenType type{};
        SourceLocation location{};
        Raychel::AssertingOptional<std::string> content{};

        Token() = default;

        explicit Token(TokenType::TokenType _t, SourceLocation _loc = {}, Raychel::AssertingOptional<std::string> _c = {})
            : type{_t}, location{_loc}, content{std::move(_c)}
        {}

        bool operator==(const Token& other) const noexcept
        {
            if (type != other.type) {
                return false;
            }
            if (content.has_value() && other.content.has_value()) {
                return *content == *other.content;
            }
            return !content.has_value() && !other.content.has_value();
        }
    };

    inline std::string token_type_to_string(RaychelScript::TokenType::TokenType type) noexcept
    {
        using TT = RaychelScript::TokenType::TokenType;

        switch (type) {
            case TT::number:
                return "NUMBER";
            case TT::declaration:
                return "DECLARATION";
            case TT::identifer:
                return "IDENTIFIER";
            case TT::conditional_header:
                return "IF";
            case TT::conditional_footer:
                return "ENDIF";
            case TT::expression_:
            case TT::arith_op_:
                RAYCHEL_TERMINATE("TokenType::expression_ and TokenType::arith_op_ cannot appear in this context!\n");
            default:
                return std::string{type};
        }
    }

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_TOKEN_H