/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H
#define RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H

#include <concepts>

namespace RaychelScript {

    template <std::floating_point T>
    class VariableDescriptor
    {
        static std::size_t s_id;

    public:

        VariableDescriptor()
            :id_{s_id++}
        {}

        [[nodiscard]] std::size_t id() const noexcept
        {
            return id_;
        }

        T& value() noexcept
        {
            return value_;
        }

        const T& value() const noexcept
        {
            return value_;
        }

    private:
        std::size_t id_;
        T value_{0};
    };

} //namespace RaychelScript

#endif //!RAYCHELSCRIPT_VARIABLE_DESCRIPTOR_H