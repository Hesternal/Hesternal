module;

#include "Copium/Core/Debug.hpp"

export module CopiumEngine.Memory;

import Copium.Core;
import CopiumEngine.Math;


export namespace Copium
{

    class Memory final
    {
    public:
        [[nodiscard]] static constexpr uint32 GetAlignedSize(uint32 unalignedSize, uint32 alignment) noexcept;
    };


    constexpr uint32 Memory::GetAlignedSize(uint32 unalignedSize, uint32 alignment) noexcept
    {
        COP_ASSERT(Math::IsPowerOfTwo(alignment) == true);
        return (unalignedSize + alignment - 1) & ~(alignment - 1);
    }

} // export namespace Copium
