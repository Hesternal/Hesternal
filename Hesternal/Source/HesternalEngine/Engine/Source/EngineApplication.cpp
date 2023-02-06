module;

#include "Hesternal/Core/Debug.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.Engine.Application;

import Hesternal.ECS;
import Hesternal.Math;

import HesternalEngine.ECS.Components;
import HesternalEngine.ECS.WorldManager;
import HesternalEngine.Engine.EngineSettings;
import HesternalEngine.Event.EventManager;
import HesternalEngine.Graphics;
import HesternalEngine.Graphics.IGraphicsDevice;
import HesternalEngine.Utils.Time;

import <utility>;


namespace Hesternal
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

        HS_LOG_TRACE("EngineApplication Init");

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

        HS_LOG_TRACE("EngineApplication Shutdown");
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

        World* const defaultWorld = WorldManager::GetDefaultWorld();
        EntityManager& entityManager = defaultWorld->GetEntityManager();

        // TODO(v.matushkin): Camera and Light initialization shouldn't be here
        //- Camera
        {
            const EngineSettings& engineSettings = EngineSettings::Get();

            constexpr float32 verticalFov = Math::ToRadians(90.0f);
            constexpr float32 near = 0.01f;
            constexpr float32 far = 1000.0f;
            const float32 aspect = float32(engineSettings.RenderWidth) / engineSettings.RenderHeight;
            const Float4x4 cameraProjection = Math::Perspective(verticalFov, aspect, near, far);

            Entity cameraEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(cameraEntity, Transform::Identity());
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
        //- Directional Light
        {
            Entity directionalLightEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(directionalLightEntity, Transform::Identity());
            entityManager.AddComponent<DirectionalLight>(directionalLightEntity, DirectionalLight{ .Color = Color::Gray()});

            defaultWorld->GetDefaultScene()->AddEntity(directionalLightEntity);
        }
        //- Point Light 1
        {
            Entity pointLightEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(pointLightEntity, Transform::FromPosition(Float3(-8.0f, 1.0f, 0.0f)));
            entityManager.AddComponent<PointLight>(pointLightEntity, PointLight{
                    .Color     = Color(0.0f, 0.3f, 0.6f),
                    .Intensity = 2.0f,
                    .Range     = 10.f,
                });

            defaultWorld->GetDefaultScene()->AddEntity(pointLightEntity);
        }
        //- Point Light 2
        {
            Entity pointLightEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(pointLightEntity, Transform::FromPosition(Float3(8.0f, 1.0f, 0.0f)));
            entityManager.AddComponent<PointLight>(pointLightEntity, PointLight{
                    .Color     = Color::Purple(),
                    .Intensity = 2.0f,
                    .Range     = 10.f,
                });

            defaultWorld->GetDefaultScene()->AddEntity(pointLightEntity);
        }
        //- Spot Light
        {
            Entity spotLightEntity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(spotLightEntity, Transform::FromPosition(Float3(0.0f, 0.05f, 0.0f)));
            entityManager.AddComponent<SpotLight>(spotLightEntity, SpotLight{
                    .Color            = Color(0.7f, 0.0f, 0.0f),
                    .Intensity        = 2.0f,
                    .Range            = 10.0f,
                    .SpotAngle        = 90.0f,
                    .InnerSpotPercent = 50.0f,
                });

            defaultWorld->GetDefaultScene()->AddEntity(spotLightEntity);
        }
    }

    void EngineApplication::_SystemsShutdown()
    {
        WorldManager::Shutdown();
        Graphics::Shutdown();
    }

} // namespace Hesternal
