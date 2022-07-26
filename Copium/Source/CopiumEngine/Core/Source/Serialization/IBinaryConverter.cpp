module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Core.Serialization.IBinaryConverter;

import CopiumEngine.Core.Object;


namespace Copium
{

    IBinaryConverter& IBinaryConverter::operator<<(Object& object)
    {
        COP_ASSERT_MSG(false, "Not implemented");
        COP_UNUSED(object);
        // object.Convert(*this);
        return *this;
    }

} // namespace Copium
