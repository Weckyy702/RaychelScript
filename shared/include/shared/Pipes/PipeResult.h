/**
* \file PipeResult.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for PipeResult class
* \date 2021-12-19
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
#ifndef RAYCHELSCRIPT_PIPE_RESULT_H
#define RAYCHELSCRIPT_PIPE_RESULT_H

#include <any>
#include <concepts>

#include "RaychelCore/AssertingGet.h"
#include "RaychelCore/AssertingOptional.h"

#define RAYCHELSCRIPT_PIPES_RETURN_IF_ERROR(name)                                                                                \
    if ((name).is_error()) {                                                                                                     \
        return (name)._error_container();                                                                                        \
    }

namespace RaychelScript {

    namespace Lexer {
        enum class LexerErrorCode;
    } // namespace Lexer

    namespace Parser {
        enum class ParserErrorCode;
    } //namespace Parser

    namespace Interpreter {
        enum class InterpreterErrorCode;
    } // namespace Interpreter

    namespace Assembler {
        enum class AssemblerErrorCode;
    } //namespace Assembler

    namespace Assembly {
        enum class ReadingErrorCode;
    } // namespace Assembly

    namespace VM {
        enum class VMErrorCode;
    } // namespace VM

    namespace NativeAssembler {
        enum class NativeAssemblerErrorCode;
    } // namespace NativeAssembler

} // namespace RaychelScript

namespace RaychelScript::Pipes {

    enum class ErrorType {
        no_error,
        lexer_error,
        parser_error,
        optimizer_error,
        interpreter_error,
        read_error,
        assembler_error,
        vm_error,
        native_assembler_error
    };

    namespace details {

        template <ErrorType type>
        using _base = std::integral_constant<ErrorType, type>;

        template <typename T>
        struct _error_type_for
        {};

        template <>
        struct _error_type_for<Lexer::LexerErrorCode> : _base<ErrorType::lexer_error>
        {};

        template <>
        struct _error_type_for<Parser::ParserErrorCode> : _base<ErrorType::parser_error>
        {};

        template <>
        struct _error_type_for<Interpreter::InterpreterErrorCode> : _base<ErrorType::interpreter_error>
        {};

        template <>
        struct _error_type_for<Assembly::ReadingErrorCode> : _base<ErrorType::read_error>
        {};

        template <>
        struct _error_type_for<Assembler::AssemblerErrorCode> : _base<ErrorType::assembler_error>
        {};

        template <>
        struct _error_type_for<VM::VMErrorCode> : _base<ErrorType::vm_error>
        {};

        template <>
        struct _error_type_for<NativeAssembler::NativeAssemblerErrorCode> : _base<ErrorType::native_assembler_error>
        {};

        class ErrorContainer
        {
        public:
            ErrorContainer() = default;

            template <typename E>
            requires std::is_enum_v<E> ErrorContainer(E value) //NOLINT we want this constructor to be implicit
                : type_{_error_type_for<E>::value}, int_value_{static_cast<std::uintmax_t>(value)}
            {}

            template <typename E>
            requires std::is_enum_v<E>
            ErrorContainer& operator=(E value) noexcept
            {
                type_ = _error_type_for<E>::value;
                int_value_ = static_cast<std::uintmax_t>(value);
                return *this;
            }

            template <typename E>
            requires std::is_enum_v<E>
            [[nodiscard]] E value() const noexcept
            {
                RAYCHEL_ASSERT(type_ == _error_type_for<E>::value);
                return static_cast<E>(int_value_);
            }

            [[nodiscard]] ErrorType type() const noexcept
            {
                return type_;
            }

        private:
            ErrorType type_{ErrorType::no_error};
            std::uintmax_t int_value_{0};
        };

        class PipeResultBase
        {
        public:
            PipeResultBase() = default;

            PipeResultBase(const ErrorContainer& error_container) //NOLINT we want this constructor to be implicit
                : value_{error_container}
            {}

            template <typename E>
            requires std::is_enum_v<E>
            explicit PipeResultBase(E value) : value_{value}
            {}

            template <typename E>
            requires std::is_enum_v<E>
            [[nodiscard]] E to_error_code() const noexcept
            {
                RAYCHEL_ASSERT(is_error())
                return value_.template value<E>();
            }

            [[nodiscard]] bool is_error() const noexcept
            {
                return error_type() != ErrorType::no_error;
            }

            [[nodiscard]] ErrorType error_type() const noexcept
            {
                return value_.type();
            }

            [[nodiscard]] ErrorContainer _error_container() const noexcept
            {
                return value_;
            }

        protected:
            [[nodiscard]] ErrorContainer& error_container() noexcept
            {
                return value_;
            }

        private:
            ErrorContainer value_{};
        };

    } // namespace details

    template <typename T>
    constexpr auto error_type_for = details::_error_type_for<T>::value;

    template <typename T>
    class PipeResult : public details::PipeResultBase
    {
        using Base = details::PipeResultBase;

    public:
        template <typename E>
        requires std::is_enum_v<E> PipeResult(E error_code) //NOLINT we want this constructor to be implicit
            : PipeResultBase(error_code)
        {}

        PipeResult(const T& value) //NOLINT we want this constructor to be implicit
            : PipeResultBase{}, maybe_value_{value}
        {}

        PipeResult(T&& value) : PipeResultBase{}, maybe_value_{std::move(value)} //NOLINT we want this constructor to be implicit
        {}

        template <typename E>
        requires std::is_enum_v<E>
        PipeResult(std::variant<E, T>&& value_or_error) //NOLINT we want this constructor to be implicit
        {
            if (const auto* ec = std::get_if<E>(&value_or_error); ec) {
                error_container() = *ec;
                return;
            }

            maybe_value_.emplace(Raychel::get<T>(std::forward<std::variant<E, T>>(value_or_error)));
        }

        PipeResult(const details::ErrorContainer& error) //NOLINT we want this constructor to be implicit
            : PipeResultBase{error}
        {
            RAYCHEL_ASSERT(error.type() != ErrorType::no_error);
        }

        [[nodiscard]] T value() const noexcept
        {
            RAYCHEL_ASSERT(!is_error());
            return maybe_value_.value();
        }

        explicit operator bool() const noexcept
        {
            return !is_error();
        }

    private:
        Raychel::AssertingOptional<T> maybe_value_{};
    };

    template <>
    class PipeResult<void> : public details::PipeResultBase
    {
        using Base = details::PipeResultBase;
        using Base::Base;
    };

    template <typename T>
    bool log_if_error(const PipeResult<T>& result) noexcept
    {
        if (!result.is_error()) {
            return false;
        }

        switch (result.error_type()) {
            case ErrorType::no_error:
                return false; //This should never happen but it's fine I guess
            case ErrorType::lexer_error:
                Logger::error("Lexer error: ", result.template to_error_code<Lexer::LexerErrorCode>(), '\n');
                break;
            case ErrorType::parser_error:
                Logger::error("Parser error: ", result.template to_error_code<Parser::ParserErrorCode>(), '\n');
                break;
            case ErrorType::optimizer_error:
                RAYCHEL_TODO("Handle optimizer error codes");
            case ErrorType::interpreter_error:
                Logger::error("Interpreter error: ", result.template to_error_code<Interpreter::InterpreterErrorCode>(), '\n');
                break;
            case ErrorType::read_error:
                Logger::error("Reading error: ", result.template to_error_code<Assembly::ReadingErrorCode>(), '\n');
                break;
            case ErrorType::assembler_error:
                Logger::error("Assembler error: ", result.template to_error_code<Assembler::AssemblerErrorCode>(), '\n');
                break;
            case ErrorType::vm_error:
                Logger::error("VM error: ", result.template to_error_code<VM::VMErrorCode>(), '\n');
                break;
            case ErrorType::native_assembler_error:
                Logger::error(
                    "Native Assembler error: ", result.template to_error_code<NativeAssembler::NativeAssemblerErrorCode>(), '\n');
        }
        return true;
    }

} //namespace RaychelScript::Pipes

#endif //!RAYCHELSCRIPT_PIPE_RESULT_H
