module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Core.Serialization.IBinaryConverter;

import HesternalEngine.Core.Object;


namespace Hesternal
{

    IBinaryConverter& IBinaryConverter::operator<<(Object& object)
    {
        HS_ASSERT_MSG(false, "Not implemented");
        HS_UNUSED(object);
        // object.Convert(*this);
        return *this;
    }

} // namespace Hesternal
