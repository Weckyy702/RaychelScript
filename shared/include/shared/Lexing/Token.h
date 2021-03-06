/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_TOKEN_H
#define RAYCHELSCRIPT_TOKEN_H

#include <RaychelCore/AssertingOptional.h>
#include <string>

#include "TokenType.h"
#include "shared/SourceLocation.h"

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

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_TOKEN_H
