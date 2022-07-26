module;

#include "Engine/Core/Debug.hpp"

#include <cmath>
#include <memory>
#include <utility>

module CopiumEngine.Application;

import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.Scene;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Math.Matrix;


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
        m_graphicsDevice = Graphics::GetGraphicsDevice();

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
        Graphics::Shutdown();
    }

    void Application::_EngineLoop()
    {
        const ShaderHandle defaultShaderHandle = Graphics::GetDefaultShader()->GetHandle();
        const Entity* sponzaRootEntity = Scene::SponzaRootEntity.get();

        Matrix4x4f sponzaTransform(
            0.005f,   0.0f,   0.0f, 0.0f,
              0.0f, 0.005f,   0.0f, 0.0f,
              0.0f,   0.0f, 0.005f, 0.0f,
              0.0f,   0.0f,   0.0f, 1.0f
        );

        // 90.0f, f32(renderWidth) / renderHeight, 0.1f, 100.0f
        // constexpr float32 fovy = 90.0f;
        // float32 aspect = float32(m_engineSettings.WindowWidth) / m_engineSettings.WindowHeight;
        // constexpr float32 zNear = 0.1f;
        // constexpr float32 zFar = 100.0f;
        // 
        // float32 tanHalfFovy = std::tan(fovy / 2.0f);
        // float32 projection00 = 1.0f / (aspect * tanHalfFovy);
        // float32 projection11 = 1.0f / (tanHalfFovy);
        // float32 projection22 = zFar / (zNear - zFar);
        // float32 projection23 = -1.0f;
        // float32 projection32 = -(zFar * zNear) / (zFar - zNear);
        // 
        // Matrix4x4f cameraProjection(
        //     projection00,         0.0f,         0.0f,         0.0f,
        //             0.0f, projection11,         0.0f,         0.0f,
        //             0.0f,         0.0f, projection22, projection23,
        //             0.0f,         0.0f, projection32,          0.0f
        // );

        Matrix4x4f cameraProjection(
            0.78125f, 0.0f,     0.0f,  0.0f,
                0.0f, 1.0f,     0.0f,  0.0f,
                0.0f, 0.0f,  -1.002f, -1.0f,
                0.0f, 0.0f, -0.2002f,  0.0f
        );

        Matrix4x4f cameraView = Matrix4x4f::Identity();

        TextureHandle currentBaseColorTexture = TextureHandle::Invalid;
        TextureHandle currentNormalTexture = TextureHandle::Invalid;

        while (true)
        {
            m_mainWindow->ProcessEvents();

            EventManager::DispatchEvents();

            if (m_mainWindow->IsClosing())
            {
                break;
            }

            m_graphicsDevice->BeginFrame(sponzaTransform, cameraView, cameraProjection);
            m_graphicsDevice->BindShader(defaultShaderHandle);

            for (const Entity& entity : sponzaRootEntity->GetChildren())
            {
                const Material* entityMaterial = entity.GetMaterial();
                const TextureHandle baseColorTextureHandle = entityMaterial->GetBaseColorMap()->GetHandle();
                const TextureHandle normalTextureHandle = entityMaterial->GetNormalMap()->GetHandle();

                if (baseColorTextureHandle != currentBaseColorTexture || normalTextureHandle != currentNormalTexture)
                {
                    currentBaseColorTexture = baseColorTextureHandle;
                    currentNormalTexture = normalTextureHandle;

                    m_graphicsDevice->BindMaterial(baseColorTextureHandle, normalTextureHandle);
                }

                m_graphicsDevice->DrawMesh(entity.GetMesh()->GetHandle());
            }

            m_graphicsDevice->EndFrame();
        }
    }

} // namespace Copium
