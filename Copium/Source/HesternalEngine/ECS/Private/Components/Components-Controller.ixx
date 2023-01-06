export module CopiumEngine.ECS.Components:Controller;

import Copium.Core;
import Copium.ECS;
import Copium.Math;


export namespace Copium
{

    struct Controller final : IComponent
    {
        float32 CameraSpeed;
        float32 CameraBoost;

        Int2    PreviousMousePosition;
        float32 Yaw;
        float32 Pitch;
    };

} // export namespace Copium
