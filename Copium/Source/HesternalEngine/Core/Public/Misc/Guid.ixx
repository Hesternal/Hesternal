export module CopiumEngine.Core.Misc.Guid;

import Copium.Core;

import <functional>;


export namespace Copium
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

} // export namespace Copium


export template<>
struct std::hash<Copium::Guid>
{
    // NOTE(v.matushkin): I'm sure this will perform well, no collisions.
    //  May be just find some hashing algorithm and use it, like microsoft stl uses FNV-1
    std::size_t operator()(const Copium::Guid& guid) const noexcept
    {
        using namespace Copium;

        std::size_t h1 = std::hash<uint64>()(static_cast<uint64>(guid.A) << 32 | guid.B);
        std::size_t h2 = std::hash<uint64>()(static_cast<uint64>(guid.C) << 32 | guid.D);
        return h1 ^ (h2 << 1);
    }
};
