module CopiumEngine.Graphics.RenderContext;

import <utility>;


namespace Copium
{

    RenderContext::RenderContext(RenderData&& renderData, IGraphicsDevice* graphicsDevice)
        : m_renderData(std::move(renderData))
        , m_graphicsDevice(graphicsDevice)
    {
    }


    void RenderContext::BeginRenderPass(RenderPassHandle renderPassHandle) const
    {
        m_graphicsDevice->BeginRenderPass(renderPassHandle);
    }

    void RenderContext::DrawEntities() const
    {
        ShaderHandle currentShaderHandle = ShaderHandle::Invalid;
        TextureHandle currentBaseColorMap = TextureHandle::Invalid;
        TextureHandle currentNormalMap = TextureHandle::Invalid;

        for (const EntityRenderData& entityRenderData : m_renderData.Entities)
        {
            const std::shared_ptr<Material>& material = m_renderData.Materials[entityRenderData.MaterialIndex];
            const ShaderHandle shaderHandle = material->GetShader()->GetHandle();
            const TextureHandle baseColorMapHandle = material->GetBaseColorMap()->GetHandle();
            const TextureHandle normalMapHandle = material->GetNormalMap()->GetHandle();

            if (currentShaderHandle != shaderHandle)
            {
                currentShaderHandle = shaderHandle;
                m_graphicsDevice->BindShader(shaderHandle);
            }

            if (baseColorMapHandle != currentBaseColorMap || normalMapHandle != currentNormalMap)
            {
                currentBaseColorMap = baseColorMapHandle;
                currentNormalMap = normalMapHandle;

                m_graphicsDevice->BindMaterial(baseColorMapHandle, normalMapHandle);
            }

            const MeshHandle meshHandle = m_renderData.Meshes[entityRenderData.MeshIndex]->GetHandle();

            m_graphicsDevice->DrawMesh(meshHandle);
        }
    }

} // namespace Copium
