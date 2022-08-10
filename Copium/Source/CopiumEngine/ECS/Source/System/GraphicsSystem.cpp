module;

#include "Engine/Core/Debug.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Graphics;

import CopiumEngine.ECS.Components;
import CopiumEngine.Engine.Application;
import CopiumEngine.Graphics;
import CopiumEngine.Platform.Window;


namespace Copium
{

    void GraphicsSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);

        Window* window = ((EngineApplication*)Application::Get())->GetMainWindow();
        IGraphicsDevice* const graphicsDevice = Graphics::GetGraphicsDevice();

        RenderTextureHandle swapchainRTHandle = graphicsDevice->GetSwapchainRenderTexture(window->GetSwapchainHandle());

        RenderTextureDesc depthRenderTextureDesc = {
            .Name       = "Engine Depth",
            .ClearValue = RenderTextureClearValue::DefaultDepthStencil(),
            .Width      = 1280,
            .Height     = 1000,
            .Format     = RenderTextureFormat::Depth32,
            .Usage      = RenderTextureUsage::Default,
        };
        m_depthRTHandle = graphicsDevice->CreateRenderTexture(depthRenderTextureDesc);

        RenderPassDesc renderPassDesc = {
            .ColorAttachments = {
                {
                    .RTHandle   = swapchainRTHandle,
                    .LoadAction = AttachmentLoadAction::Clear,
                }
            },
            .DepthStencilAttachment = AttachmentDesc{
                .RTHandle   = m_depthRTHandle,
                .LoadAction = AttachmentLoadAction::Clear,
            },
            .Subpass = {
                .ColorAttachmentIndices    = {0},
                .UseDepthStencilAttachment = true,
            },
        };
        m_renderPassHandle = graphicsDevice->CreateRenderPass(renderPassDesc);
    }

    void GraphicsSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);

        IGraphicsDevice* const graphicsDevice = Graphics::GetGraphicsDevice();
        graphicsDevice->DestroyRenderPass(m_renderPassHandle);
        graphicsDevice->DestroyRenderTexture(m_depthRTHandle);
    }

    void GraphicsSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto cameraView = entityManager.GetView<const LocalToWorld, const Camera>();
        COP_ASSERT_MSG(cameraView.size_hint() == 1, "The scene must have at least and only 1 camera");
        const auto [cameraEntity, cameraLocalToWorld, camera] = *cameraView.each().begin();

        const auto renderMeshView = entityManager.GetView<const LocalToWorld, const RenderMesh>();

        TextureHandle currentBaseColorMap = TextureHandle::Invalid;
        TextureHandle currentNormalMap = TextureHandle::Invalid;

        IGraphicsDevice* const graphicsDevice = Graphics::GetGraphicsDevice();
        graphicsDevice->BeginFrame(Float4x4::Scale(0.005f), cameraLocalToWorld.Value, camera.Projection);
        graphicsDevice->BeginRenderPass(m_renderPassHandle);

        graphicsDevice->BindShader(Graphics::GetDefaultShader()->GetHandle());

        for (const auto [entity, localToWorld, renderMesh] : renderMeshView.each())
        {
            const TextureHandle baseColorMapHandle = renderMesh.Material->GetBaseColorMap()->GetHandle();
            const TextureHandle normalMapHandle = renderMesh.Material->GetNormalMap()->GetHandle();

            if (baseColorMapHandle != currentBaseColorMap || normalMapHandle != currentNormalMap)
            {
                currentBaseColorMap = baseColorMapHandle;
                currentNormalMap = normalMapHandle;

                graphicsDevice->BindMaterial(baseColorMapHandle, normalMapHandle);
            }

            graphicsDevice->DrawMesh(renderMesh.Mesh->GetHandle());
        }

        graphicsDevice->EndFrame();
    }

} // namespace Copium
