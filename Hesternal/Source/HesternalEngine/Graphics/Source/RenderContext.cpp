module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Graphics.RenderContext;

import Hesternal.Core;
import Hesternal.Math;


namespace
{
    using namespace Hesternal;


    struct PerCamera final
    {
        Float4x4 _CameraView;
        Float4x4 _CameraProjection;
        Float4x4 _CameraViewProjection;
    };

    // NOTE(v.matushkin): Will be padded to 256 bytes by GraphicsBuffer
    struct PerDraw final
    {
        Float4x4 _LocalToWorld;
    };


    static constexpr int32 k_PerCameraSizeInBytes = sizeof(PerCamera);
    static constexpr int32 k_PerDrawSizeInBytes   = sizeof(PerDraw);
    // NOTE(v.matushkin): 1500 needed for Bistro, 2500 for SanMiguel
    static constexpr int32 k_PerDrawElements      = 2500;

    static constexpr uint32 k_PerCameraSlot = 0;
    static constexpr uint32 k_PerDrawSlot   = 1;

} // namespace


namespace Hesternal
{

    RenderContext::RenderContext()
        : m_perCameraBuffer(GraphicsBufferDesc::Constant(1, k_PerCameraSizeInBytes))
        , m_perDrawBuffers(GraphicsBufferDesc::Constant(k_PerDrawElements, k_PerDrawSizeInBytes))
    {}


    void RenderContext::NewFrame()
    {
        // NOTE(v.matushkin): Assuming I need to update these every frame, otherwise it's a waste
        //- Update PerCamera constant buffer
        {
            std::span<uint8> perCameraBufferData = m_commandBuffer.MapBuffer(m_perCameraBuffer);
            std::memcpy(perCameraBufferData.data(), &m_renderData.Camera, k_PerCameraSizeInBytes);
            m_commandBuffer.UnmapBuffer(m_perCameraBuffer);
        }
        //- Update PerDraw constant buffers
        {
            const uint32 entitiesCount = static_cast<uint32>(m_renderData.Entities.size());

            HS_ASSERT(entitiesCount <= k_PerDrawElements);

            PerDraw perDraw;
            const uint32 perDrawElementSizeInBytes = m_perDrawBuffers.GetElementSize();

            std::span<uint8> perDrawBufferData = m_commandBuffer.MapBuffer(m_perDrawBuffers);
            uint8* perDrawBufferDataPtr = perDrawBufferData.data();

            for (uint32 i = 0; i < entitiesCount; i++)
            {
                perDraw._LocalToWorld = m_renderData.Entities[i].LocalToWorld;

                std::memcpy(perDrawBufferDataPtr, &perDraw, k_PerDrawSizeInBytes);

                perDrawBufferDataPtr += perDrawElementSizeInBytes;
            }

            m_commandBuffer.UnmapBuffer(m_perDrawBuffers);
        }

        m_commandBuffer.BindConstantBuffer(&m_perCameraBuffer, k_PerCameraSlot);
    }


    void RenderContext::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        m_commandBuffer.BeginRenderPass(renderPassHandle);
    }

    void RenderContext::DrawEntities()
    {
        TextureHandle currentBaseColorMap = TextureHandle::Invalid;
        TextureHandle currentMetallicMap  = TextureHandle::Invalid;
        TextureHandle currentRoughnessMap = TextureHandle::Invalid;
        TextureHandle currentNormalMap    = TextureHandle::Invalid;

        const uint32 entitiesCount = static_cast<uint32>(m_renderData.Entities.size());

        for (uint32 i = 0; i < entitiesCount; i++)
        {
            const EntityRenderData& entityRenderData = m_renderData.Entities[i];

            const Material& material = *m_renderData.Materials[entityRenderData.MaterialIndex];
            const Texture* const baseColorMap = material.GetBaseColorMap().get();
            const Texture* const metallicMap  = material.GetMetallicMap().get();
            const Texture* const roughnessMap = material.GetRoughnessMap().get();
            const Texture* const normalMap    = material.GetNormalMap().get();
            const TextureHandle baseColorMapHandle = baseColorMap->GetHandle();
            const TextureHandle metallicHandle     = metallicMap->GetHandle();
            const TextureHandle roughnessMapHandle = roughnessMap->GetHandle();
            const TextureHandle normalMapHandle    = normalMap->GetHandle();

            m_commandBuffer.BindShader(material.GetShader().get());

            if (baseColorMapHandle != currentBaseColorMap || metallicHandle != currentMetallicMap
                || roughnessMapHandle != currentRoughnessMap || normalMapHandle != currentNormalMap)
            {
                currentBaseColorMap = baseColorMapHandle;
                currentMetallicMap  = metallicHandle;
                currentRoughnessMap = roughnessMapHandle;
                currentNormalMap    = normalMapHandle;

                m_commandBuffer.BindMaterial(baseColorMap, metallicMap, roughnessMap, normalMap);
            }

            m_commandBuffer.BindConstantBuffer(&m_perDrawBuffers, k_PerDrawSlot, i);
            m_commandBuffer.DrawMesh(*m_renderData.Meshes[entityRenderData.MeshIndex]);
        }
    }

} // namespace Hesternal
