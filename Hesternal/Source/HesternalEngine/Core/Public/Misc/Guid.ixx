export module HesternalEngine.Core.Misc.Guid;

import Hesternal.Core;

import <functional>;


export namespace Hesternal
{

    struct Guid final
    {
        uint32 A;
        uint32 B;
        uint32 C;
        uint32 D;

        bool operator==(const Guid& guid) const
        {
            return A == guid.A && B == guid.B && C == guid.C && D == guid.D;
        }

        static Guid Generate();
    };

} // export namespace Hesternal


export template<>
struct std::hash<Hesternal::Guid>
{
    // NOTE(v.matushkin): I'm sure this will perform well, no collisions.
    //  May be just find some hashing algorithm and use it, like microsoft stl uses FNV-1
    std::size_t operator()(const Hesternal::Guid& guid) const noexcept
    {
        using namespace Hesternal;

        std::size_t h1 = std::hash<uint64>()(static_cast<uint64>(guid.A) << 32 | guid.B);
        std::size_t h2 = std::hash<uint64>()(static_cast<uint64>(guid.C) << 32 | guid.D);
        return h1 ^ (h2 << 1);
    }
};
