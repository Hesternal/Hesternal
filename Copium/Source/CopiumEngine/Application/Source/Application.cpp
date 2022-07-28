module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Application;

import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.Scene;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Math;

import <memory>;
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

        Float4x4 sponzaTransform = Float4x4::Scale(0.005f);

        constexpr float32 verticalFov = Math::Radians(90.0f);
        constexpr float32 near = 0.1f;
        constexpr float32 far = 100.0f;
        float32 aspect = float32(m_engineSettings.WindowWidth) / m_engineSettings.WindowHeight;

        Float4x4 cameraProjection = Float4x4::Perspective(verticalFov, aspect, near, far);
        Float4x4 cameraView = Float4x4::Identity();

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
