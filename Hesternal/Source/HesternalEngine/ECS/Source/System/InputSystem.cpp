module;

#include "Hesternal/Core/Defines.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.System.Input;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.ECS.Components;
import HesternalEngine.Event.Input;
import HesternalEngine.Utils.Time;


namespace
{
    using namespace Hesternal;


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
            translation.X -= 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::D))
        {
            translation.X += 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::Q))
        {
            translation.Y -= 1.0f;
        }
        if (Input::IsButtonPressed(KeyboardButton::E))
        {
            translation.Y += 1.0f;
        }

        if (Input::IsButtonPressed(KeyboardButton::LeftShift))
        {
            cameraSpeed *= cameraBoost;
        }

        translation *= static_cast<float32>(Time::GetDelta() * cameraSpeed);

        return translation;
    }

} // namespace


namespace Hesternal
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
        const auto cameraQuery = entityManager.GetView<Transform, Controller>();

        const Int2 currentMousePosition = Input::GetMousePosition();
        const float32 deltaTime = static_cast<float32>(Time::GetDelta());

        for (const auto&& [cameraEntity, cameraTransform, cameraController] : cameraQuery.each())
        {
            if (Input::IsButtonPressed(MouseButton::Right))
            {
                if (currentMousePosition != cameraController.PreviousMousePosition)
                {
                    const Int2 mousePositionDelta = currentMousePosition - cameraController.PreviousMousePosition;

                    cameraController.Pitch += mousePositionDelta.Y * deltaTime;
                    cameraController.Yaw += mousePositionDelta.X * deltaTime;

                    cameraTransform.Rotation = Quaternion::EulerZXY(cameraController.Pitch, cameraController.Yaw, 0.0f);
                }
            }

            cameraController.PreviousMousePosition = currentMousePosition;

            const Float3 translation = GetCameraTranslation(cameraController.CameraSpeed, cameraController.CameraBoost);
            cameraTransform.Position += Quaternion::Mul(cameraTransform.Rotation, translation);
        }
    }

} // namespace Hesternal
