/**
* \file Scope.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Scope class
* \date 2022-07-25
*
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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
#ifndef RAYCHELSCRIPT_SCOPE_H
#define RAYCHELSCRIPT_SCOPE_H

#include <concepts>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace RaychelScript {

    namespace details {

        template <typename T>
        struct scope_data
        {
            using type = T;
        };

        template <>
        struct scope_data<void>
        {
            struct empty
            {};

            using type = empty;
        };

        template <typename T>
        using scope_data_t = typename scope_data<T>::type;

    } // namespace details

    template <typename Reffered, typename Key = std::string, typename Data = void>
    struct BasicScope
    {
        bool inherits_from_parent_scope;

        details::scope_data_t<Data> scope_data{};
        std::unordered_map<Key, Reffered> lookup_table{};
    };

    enum class IterationDecision {
        continue_,
        break_,
    };

    //TODO: use ranges
    template <typename Referred, typename Key, typename Data, std::invocable<const BasicScope<Referred, Key, Data>&> F>
    bool for_each_scope(const std::vector<BasicScope<Referred, Key, Data>>& scopes, F&& f) noexcept
    {
        if (scopes.empty()) {
            return false;
        }
        auto scope_it = std::prev(scopes.end());
        do {
            const auto& scope = *scope_it;

            if (f(scope) != IterationDecision::continue_) {
                return true;
            }

            if (!scope.inherits_from_parent_scope) {
                break; //we need to stop traversing the scope chain here
            }
        } while (scope_it-- != scopes.begin());
        return false;
    }

    template <typename Referred, typename Key, typename Data>
    [[nodiscard]] bool has_identifier(const std::vector<BasicScope<Referred, Key, Data>>& scopes, const Key& name) noexcept
    {
        return for_each_scope(scopes, [&](const BasicScope<Referred, Key, Data>& scope) {
            if (scope.lookup_table.find(name) != scope.lookup_table.end()) {
                return IterationDecision::break_;
            }
            return IterationDecision::continue_;
        });
    }

    template <typename Referred, typename Key, typename Data>
    [[nodiscard]] std::optional<Referred>
    find_identifier(const std::vector<BasicScope<Referred, Key, Data>>& scopes, const Key& name) noexcept
    {
        std::optional<Referred> maybe_descriptor{};
        for_each_scope(scopes, [&](const BasicScope<Referred, Key, Data>& scope) {
            if (const auto it = scope.lookup_table.find(name); it != scope.lookup_table.end()) {
                maybe_descriptor = it->second;
                return IterationDecision::break_;
            }
            return IterationDecision::continue_;
        });

        return maybe_descriptor;
    }

    template <typename T, typename... ExtraArgs>
    requires(requires(T& t) {
        t.push_scope(std::declval<bool>(), std::declval<ExtraArgs>()...);
        t.pop_scope(std::declval<ExtraArgs>()...);
    }) class ScopePusher
    {
    public:
        explicit ScopePusher(T& t, bool inherits_from_parent_scope, ExtraArgs... extra_args) : pop_args_{t, extra_args...}
        {
            t.push_scope(inherits_from_parent_scope, std::forward<ExtraArgs>(extra_args)...);
        }

        ~ScopePusher() noexcept
        {
            std::apply(&T::pop_scope, std::move(pop_args_));
        }

    private:
        std::tuple<T&, ExtraArgs...> pop_args_;
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_SCOPE_H
