/**
* \file ConfigBlock.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Source file for parsing functions regarding the [[config]] block of the program. Just for Project structure
* \date 2021-10-19
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

#include <array>
#include <vector>

#include "shared/AST/AST.h"
#include "shared/Lexing/Token.h"

using SourceTokens = std::vector<std::vector<RaychelScript::Token>>;
using LineTokens = std::vector<RaychelScript::Token>;

namespace RaychelScript::Parser {

    [[nodiscard]] static bool
    parse_config_entry(const LineTokens& entry_tokens, std::vector<std::string>& config_entries) noexcept
    {
        if (!config_entries.empty()) {
            Logger::error("Duplicate config entry!\n");
            return false;
        }
        bool has_failed{false};

        std::for_each(
            std::next(entry_tokens.begin()),
            entry_tokens.end(),
            [&has_failed, &config_entries, index = 0U](const Token& token) mutable {
                if (has_failed) {
                    return;
                }

                //every second token has to be an identifier
                if (index % 2U == 0 && (token.type != TokenType::identifer && token.type != TokenType::number)) {
                    Logger::error(token.location, ": invalid token in config entry!\n");
                    has_failed = true;
                    return;
                }
                if (token.type == TokenType::comma) {
                    index++;
                    return;
                }

                config_entries.push_back(*token.content);
                index++;
            });

        return !has_failed;
    }

    [[nodiscard]] static bool parse_input_entry(const LineTokens& entry_tokens, ConfigBlock& config_block) noexcept
    {
        return parse_config_entry(entry_tokens, config_block.input_identifiers);
    }

    [[nodiscard]] static bool parse_output_entry(const LineTokens& entry_tokens, ConfigBlock& config_block) noexcept
    {
        return parse_config_entry(entry_tokens, config_block.output_identifiers);
    }

    [[nodiscard]] static bool parse_config_entry(const LineTokens& line, ConfigBlock& config_block) noexcept
    {
        //minimal config entry: name
        if (line.empty()) {
            Logger::error("Invalid config line! Every config line must be at least one token long!\n");
            return false;
        }

        //a config entry can only ever be ID (ID/NUM), (ID/NUM), (ID/NUM), ...
        const bool valid = std::all_of(line.begin(), line.end(), [](const Token& token) {
            return token.type == TokenType::identifer || token.type == TokenType::comma || token.type == TokenType::number;
        });

        const auto& first_token = line.at(0);

        if (!valid) {
            Logger::error(
                first_token.location, ": invalid config entry! Config entries can only contain identifiers and comma!\n");
            return false;
        }

        if (first_token.type != TokenType::identifer) {
            Logger::error(
                first_token.location, ": invalid config entry! First token in every config line must be an identifer!\n");
            return false;
        }

        if (*first_token.content == "input") {
            return parse_input_entry(line, config_block);
        }

        if (*first_token.content == "output") {
            return parse_output_entry(line, config_block);
        }

        return parse_config_entry(line, config_block.config_vars[*first_token.content]);
    }

    [[nodiscard]] bool parse_config_block(const SourceTokens& config_tokens, AST& ast) noexcept
    {
        bool has_failed = false;
        std::for_each(config_tokens.begin(), config_tokens.end(), [&has_failed, &ast](const auto& line) {
            if (!has_failed) {
                has_failed = !parse_config_entry(line, ast.config_block);
            }
        });

        return !has_failed;
    }
} // namespace RaychelScript::Parser
