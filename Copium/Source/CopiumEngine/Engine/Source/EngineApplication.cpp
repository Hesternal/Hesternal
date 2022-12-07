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
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Math;
import CopiumEngine.Graphics;
import CopiumEngine.Graphics.IGraphicsDevice;
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
        OnEngine_Init(argc, argv);

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

        WorldManager::GetDefaultWorld()->Update();
        OnEngine_Update();

        {
            IGraphicsDevice* const graphicsDevice = Graphics::GetGraphicsDevice();
            RenderContext& renderContext = Graphics::GetRenderContext();

            graphicsDevice->BeginFrame();

            renderContext.NewFrame();
            OnEngine_Render(renderContext);

            graphicsDevice->EndFrame();
        }
    }


    void EngineApplication::_SystemsInit()
    {
        Time::Init();
        Graphics::Init();
        WorldManager::Init();

        // TODO(v.matushkin): Camera initialization shouldn't be here
        {
            EngineSettings& engineSettings = EngineSettings::Get();

            constexpr float32 verticalFov = Math::ToRadians(90.0f);
            constexpr float32 near = 0.01f;
            constexpr float32 far = 1000.0f;
            const float32 aspect = float32(engineSettings.RenderWidth) / engineSettings.RenderHeight;
            const Float4x4 cameraProjection = Math::Perspective(verticalFov, aspect, near, far);

            World* const defaultWorld = WorldManager::GetDefaultWorld();
            EntityManager& entityManager = defaultWorld->GetEntityManager();

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

            defaultWorld->GetDefaultScene()->AddEntity(cameraEntity);
        }
    }

    void EngineApplication::_SystemsShutdown()
    {
        WorldManager::Shutdown();
        Graphics::Shutdown();
    }

} // namespace Copium
