/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_INDENT_HANDLER_H
#define RAYCHELSCRIPT_INDENT_HANDLER_H

#include <string_view>

#include <RaychelCore/ClassMacros.h>
#include <RaychelCore/Raychel_assert.h>

namespace RaychelScript {
    class IndentHandler
    {
        //NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables): false positive?
        static std::size_t indent_;

    public:
        IndentHandler()
        {
            RAYCHEL_ASSERT(++indent_ != 50);
        }

        RAYCHEL_MAKE_NONCOPY_NONMOVE(IndentHandler)

        static void reset_indent() noexcept
        {
            indent_ = 0;
        }

        //NOLINTNEXTLINE(readability-convert-member-functions-to-static)
        [[nodiscard]] auto indent() const noexcept
        {
            //this is very very stupid
            return std::string_view{
                "--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..",
                (indent_ - 1) * 2};
        }

        ~IndentHandler() noexcept
        {
            indent_--;
        }
    };

} // namespace RaychelScript

#endif //!RAYCHELSCRIPT_INDENT_HANDLER_H