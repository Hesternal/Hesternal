module;

// #include "Hesternal/Core/Debug.hpp"
#include "Hesternal/Core/Defines.hpp"

export module HesternalEngine.Memory;

import Hesternal.Core;
import Hesternal.Math;


export namespace Hesternal
{

    class Memory final
    {
    public:
        [[nodiscard]] static constexpr uint32 GetAlignedSize(uint32 unalignedSize, uint32 alignment) noexcept;
    };


    constexpr uint32 Memory::GetAlignedSize(uint32 unalignedSize, uint32 alignment) noexcept
    {
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        // HS_ASSERT(Math::IsPowerOfTwo(alignment));
#if HS_ENABLE_ASSERTS
        if (Math::IsPowerOfTwo(alignment) == false)
        {
            std::abort();
        }
#endif
        return (unalignedSize + alignment - 1) & ~(alignment - 1);
    }

} // export namespace Hesternal
