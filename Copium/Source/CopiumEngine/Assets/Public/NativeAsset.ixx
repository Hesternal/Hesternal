module;

#include "Engine/Core/Defines.hpp"
#include "EngineCht/NativeAsset.cht.hpp"

#include <vector>

export module CopiumEngine.Assets.NativeAsset;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    // NOTE(v.matushkin): This is probably a temporary class,
    //  until I decide how I wont to group Model meshes and materials
    CHT_STRUCT()
    struct ModelAsset
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        std::vector<MeshDesc> Meshes;
    };

} // export namespace Copium
