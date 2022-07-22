module;

#include "Engine/Core/Debug.hpp"

#include <memory>
#include <utility>

module CopiumEngine.Application;

import CopiumEngine.Event.EventManager;


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

        ShaderDesc shaderDesc;
        shaderDesc.VertexSource = R"(
            struct Attributes
            {
                uint vertexID : SV_VertexID;
            };
            struct Varyings
            {
                float4 positionCS : SV_POSITION;
                float3 color      : TEXCOORD0;
            };

            static const float2 vertices[] = {
                 0.0f,  0.5f,
                 0.5f, -0.5f,
                -0.5f, -0.5f,
            };

            static const float3 colors[] = {
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            };

            Varyings main(Attributes IN)
            {
                Varyings OUT;
                OUT.positionCS = float4(vertices[IN.vertexID], 0, 1);
                OUT.color      = colors[IN.vertexID];
                return OUT;
            }
        )";
        shaderDesc.FragmentSource = R"(
            struct Varyings
            {
                float4 positionCS : SV_POSITION;
                float3 color      : TEXCOORD0;
            };

            float4 main(Varyings IN) : SV_Target
            {
                return float4(IN.color, 1);
            }
        )";
        m_triangleShaderHandle = m_graphicsDevice->CreateShader(shaderDesc);

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

        m_graphicsDevice->DestroyShader(m_triangleShaderHandle);
        m_mainWindow.reset();
        Graphics::Shutdown();
    }

    void Application::_EngineLoop()
    {
        while (true)
        {
            m_mainWindow->ProcessEvents();

            EventManager::DispatchEvents();

            if (m_mainWindow->IsClosing())
            {
                break;
            }

            m_graphicsDevice->BeginFrame();
            m_graphicsDevice->BindShader(m_triangleShaderHandle);
            m_graphicsDevice->DrawProcedural(3);
            m_graphicsDevice->EndFrame();
        }
    }

} // namespace Copium
