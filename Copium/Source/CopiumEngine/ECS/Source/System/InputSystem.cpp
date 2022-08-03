module;

#include "Engine/Core/Debug.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Input;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.ECS.Components;
import CopiumEngine.Event.Input;
import CopiumEngine.Math;
import CopiumEngine.Utils.Time;


namespace
{

    using namespace Copium;


    [[nodiscard]] static Float3 GetCameraTranslation(float32 cameraSpeed, float32 cameraBoost)
    {
        Float3 translation = Float3::Zero();

        if (Input::IsButtonPressed(KeyboardButton::W))
        {
            translation.Z += 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::S))
        {
            translation.Z -= 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::A))
        {
            translation.X += 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::D))
        {
            translation.X -= 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::Q))
        {
            translation.Y += 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::E))
        {
            translation.Y -= 1.0f;
        }

        if (Input::IsButtonPressed(KeyboardButton::LeftShit))
        {
            cameraSpeed *= cameraBoost;
        }

        translation *= static_cast<float32>(Time::GetDelta() * cameraSpeed);

        return translation;
    }

} // namespace


namespace Copium
{

    void InputSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void InputSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void InputSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto cameraQuery = entityManager.GetView<Translation, Rotation, LocalToWorld, Controller>();

        const Int2 currentMousePosition = Input::GetMousePosition();
        const float32 deltaTime = static_cast<float32>(Time::GetDelta());

        for (const auto [cameraEntity, cameraTranslation, cameraRotation, cameraLocalToWorld, cameraController] : cameraQuery.each())
        {
            if (Input::IsButtonPressed(MouseButton::Right))
            {
                if (currentMousePosition != cameraController.PreviousMousePosition)
                {
                    const Int2 mousePositionDelta = currentMousePosition - cameraController.PreviousMousePosition;

                    cameraController.Pitch += mousePositionDelta.Y * deltaTime;
                    cameraController.Yaw += mousePositionDelta.X * deltaTime;

                    Quaternion rotationPitch = Quaternion::AxisAngle(Math::Left(), cameraController.Pitch);
                    Quaternion rotationYaw = Quaternion::AxisAngle(Math::Up(), cameraController.Yaw);
                    cameraRotation.Value = Quaternion::Mul(rotationPitch, rotationYaw);
                }
            }

            cameraController.PreviousMousePosition = currentMousePosition;

            Float3 translation = Quaternion::Mul(GetCameraTranslation(cameraController.CameraSpeed, cameraController.CameraBoost), cameraRotation.Value);
            cameraTranslation.Value += translation;

            cameraLocalToWorld.Value = Float4x4::TR(cameraTranslation.Value, cameraRotation.Value);
        }
    }

} // namespace Copium
