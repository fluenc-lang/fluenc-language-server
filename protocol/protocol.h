#pragma once

#include <nlohmann/json.hpp>

namespace std
{
    template <typename T>
    inline void to_json(nlohmann::json &target, const optional<T> &source)
    {
        if (source)
        {
            target = *source;
        }
    }

    template <typename T>
    inline void from_json(const nlohmann::json &source, optional<T> &target)
    {
        T value;

        source.get_to(value);

        target = value;
    }
}
