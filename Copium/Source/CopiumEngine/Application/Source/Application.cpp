module;

#include "Engine/Core/Debug.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.Application;

import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.EntityManager;
import CopiumEngine.ECS.Scene;
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Math;
import CopiumEngine.Graphics;

import <utility>;


namespace Copium
{

    Application::Application()
        : m_engineInitialized(false)
        , m_engineClosed(false)
    {
        Debug::Init();
        COP_LOG_TRACE("Application()");
    }

    Application::~Application()
    {
        COP_LOG_TRACE("~Application()");
        _EngineShutdown();
    }


    void Application::Run()
    {
        InitSettings(m_engineSettings);

        _EngineInit();
        InitSystems();

        _EngineLoop();

        ShutdownSystems();
        _EngineShutdown();
    }


    void Application::_EngineInit()
    {
        Graphics::Init();
        WorldManager::Init();

        WindowDesc windowDesc = {
            .Title  = m_engineSettings.WindowTitle,
            .Width  = m_engineSettings.WindowWidth,
            .Height = m_engineSettings.WindowHeight,
        };
        m_mainWindow = std::make_unique<Window>(std::move(windowDesc));

        m_engineInitialized = true;
    }

    void Application::_EngineShutdown()
    {
        if (m_engineInitialized == false || m_engineClosed)
        {
            return;
        }

        m_engineClosed = true;

        m_mainWindow.reset();

        WorldManager::Shutdown();
        Graphics::Shutdown();
    }

    void Application::_EngineLoop()
    {
        World* defaultWorld = WorldManager::GetDefaultWorld();
        EntityManager& entityManager = defaultWorld->GetEntityManager();

        Float4x4 sponzaTransform = Float4x4::Scale(0.005f);

        constexpr float32 verticalFov = Math::Radians(90.0f);
        constexpr float32 near = 0.1f;
        constexpr float32 far = 100.0f;
        float32 aspect = float32(m_engineSettings.WindowWidth) / m_engineSettings.WindowHeight;

        Float4x4 cameraProjection = Float4x4::Perspective(verticalFov, aspect, near, far);
        Float4x4 cameraView = Float4x4::Identity();

        Entity cameraEntity = entityManager.CreateEntity();
        entityManager.AddComponent<LocalToWorld>(cameraEntity, LocalToWorld{ .Value = cameraView });
        entityManager.AddComponent<Camera>(cameraEntity, Camera{ .Projection = cameraProjection });

        while (true)
        {
            m_mainWindow->ProcessEvents();

            EventManager::DispatchEvents();

            if (m_mainWindow->IsClosing())
            {
                break;
            }

            defaultWorld->Update();
        }

        entityManager.DestroyEntity(cameraEntity);
    }

} // namespace Copium
