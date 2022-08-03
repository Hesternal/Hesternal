export module CopiumEngine.ECS.Components:Controller;

import :IComponent;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Math;

import <memory>;


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
