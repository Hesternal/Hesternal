module;

#include "Engine/Core/Debug.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.Engine.Application;

import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.EntityManager;
import CopiumEngine.ECS.Scene;
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Math;
import CopiumEngine.Graphics;
import CopiumEngine.Utils.Time;

import <utility>;


namespace Copium
{

    EngineApplication::EngineApplication()
        : m_engineInitialized(false)
        , m_engineClosed(false)
    {
    }

    EngineApplication::~EngineApplication()
    {
        OnApplication_Shutdown();
    }


    void EngineApplication::OnApplication_Init(int32 argc, const char* const* argv)
    {
        if (m_engineInitialized)
        {
            return;
        }

        m_engineInitialized = true;
        m_engineClosed = false;

        COP_LOG_TRACE("EngineApplication Init");

        //- Derived App Init
        OnEngine_Init(argc, argv, m_engineSettings);

        //- Systems Init
        _SystemsInit();
        OnEngine_SystemsInit();
    }

    void EngineApplication::OnApplication_Shutdown()
    {
        if (m_engineInitialized == false || m_engineClosed)
        {
            return;
        }

        m_engineInitialized = false;
        m_engineClosed = true;

        OnEngine_Shutdown();
        _SystemsShutdown();

        COP_LOG_TRACE("EngineApplication Shutdown");
    }

    void EngineApplication::OnApplication_Update()
    {
        Time::Update();

        EventManager::DispatchEvents();

        if (m_mainWindow->IsClosing())
        {
            Close();
            return;
        }

        WorldManager::GetDefaultWorld()->Update();

        OnEngine_Update();
    }


    void EngineApplication::_SystemsInit()
    {
        Time::Init();
        Graphics::Init();
        WorldManager::Init();

        WindowDesc windowDesc = {
            .Title  = m_engineSettings.WindowTitle,
            .Width  = m_engineSettings.WindowWidth,
            .Height = m_engineSettings.WindowHeight,
        };
        m_mainWindow = std::make_unique<Window>(std::move(windowDesc));

        // TODO(v.matushkin): Camera initialization shouldn't be here
        {
            constexpr float32 verticalFov = Math::Radians(90.0f);
            constexpr float32 near = 0.01f;
            constexpr float32 far = 100.0f;
            float32 aspect = float32(m_engineSettings.WindowWidth) / m_engineSettings.WindowHeight;

            Float4x4 cameraProjection = Float4x4::Perspective(verticalFov, aspect, near, far);

            EntityManager& entityManager = WorldManager::GetDefaultWorld()->GetEntityManager();
            Entity cameraEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Translation>(cameraEntity, Translation{ .Value = Float3::Zero() });
            entityManager.AddComponent<Rotation>(cameraEntity, Rotation{ .Value = Quaternion::Identity() });
            entityManager.AddComponent<LocalToWorld>(cameraEntity, LocalToWorld{ .Value = Float4x4::Identity() });
            entityManager.AddComponent<Controller>(cameraEntity, Controller{
                .CameraSpeed           = 2.0f,
                .CameraBoost           = 5.0f,
                .PreviousMousePosition = Int2::Zero(),
                .Yaw                   = 0.0f,
                .Pitch                 = 0.0f,
            });
            entityManager.AddComponent<Camera>(cameraEntity, Camera{ .Projection = cameraProjection });
        }
    }

    void EngineApplication::_SystemsShutdown()
    {
        m_mainWindow.reset();

        WorldManager::Shutdown();
        Graphics::Shutdown();
    }

} // namespace Copium
