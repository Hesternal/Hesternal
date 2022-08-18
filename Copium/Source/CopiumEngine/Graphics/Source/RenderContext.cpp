module CopiumEngine.Graphics.RenderContext;

import <utility>;


namespace Copium
{

    RenderContext::RenderContext(RenderData&& renderData, CommandBuffer&& commandBuffer)
        : m_renderData(std::move(renderData))
        , m_commandBuffer(std::move(commandBuffer))
    {
    }


    void RenderContext::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        m_commandBuffer.BeginRenderPass(renderPassHandle);
    }

    void RenderContext::DrawEntities()
    {
        TextureHandle currentBaseColorMap = TextureHandle::Invalid;
        TextureHandle currentNormalMap = TextureHandle::Invalid;

        for (const EntityRenderData& entityRenderData : m_renderData.Entities)
        {
            const std::shared_ptr<Material>& material = m_renderData.Materials[entityRenderData.MaterialIndex];
            const Texture* baseColorMap = material->GetBaseColorMap().get();
            const Texture* normalMap = material->GetNormalMap().get();
            const TextureHandle baseColorMapHandle = baseColorMap->GetHandle();
            const TextureHandle normalMapHandle = normalMap->GetHandle();

            m_commandBuffer.BindShader(material->GetShader().get());

            if (baseColorMapHandle != currentBaseColorMap || normalMapHandle != currentNormalMap)
            {
                currentBaseColorMap = baseColorMapHandle;
                currentNormalMap = normalMapHandle;

                m_commandBuffer.BindMaterial(baseColorMap, normalMap);
            }

            const Mesh* mesh = m_renderData.Meshes[entityRenderData.MeshIndex].get();
            m_commandBuffer.DrawMesh(mesh);
        }
    }

} // namespace Copium
