#pragma once

#include <type_traits>


namespace Copium
{

    template<class TEnum>
    struct IsChtEnum : std::false_type {};

} // namespace Copium
