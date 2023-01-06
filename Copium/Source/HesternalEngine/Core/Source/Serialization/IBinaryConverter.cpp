module;

#include "Copium/Core/Debug.hpp"

module HesternalEngine.Core.Serialization.IBinaryConverter;

import HesternalEngine.Core.Object;


namespace Hesternal
{

    IBinaryConverter& IBinaryConverter::operator<<(Object& object)
    {
        COP_ASSERT_MSG(false, "Not implemented");
        COP_UNUSED(object);
        // object.Convert(*this);
        return *this;
    }

} // namespace Hesternal
