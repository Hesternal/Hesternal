export module HesternalEngine.ECS.Components:Controller;

import Hesternal.Core;
import Hesternal.ECS;
import Hesternal.Math;


export namespace Hesternal
{

    struct Controller final : IComponent
    {
        float32 CameraSpeed;
        float32 CameraBoost;

        Int2    PreviousMousePosition;
        float32 Yaw;
        float32 Pitch;
    };

} // export namespace Hesternal
