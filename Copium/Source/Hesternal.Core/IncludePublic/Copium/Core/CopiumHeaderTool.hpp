#pragma once

#include <type_traits>


namespace Hesternal
{

    template<class TEnum>
    struct IsChtEnum : std::false_type {};

} // namespace Hesternal
