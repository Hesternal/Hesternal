module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Graphics.RenderContext;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Math;

import <utility>;


namespace
{
    using namespace Copium;


    struct PerCamera final
    {
        Float4x4 _CameraView;
        Float4x4 _CameraProjection;
    };

    // NOTE(v.matushkin): Will be padded to 256 bytes by GraphicsBuffer
    struct PerDraw final
    {
        Float4x4 _LocalToWorld;
    };


    static constexpr int32 k_PerCameraSizeInBytes = sizeof(PerCamera);
    static constexpr int32 k_PerDrawSizeInBytes   = sizeof(PerDraw);
    static constexpr int32 k_PerDrawElements      = 500;

    static constexpr uint32 k_PerCameraSlot = 0;
    static constexpr uint32 k_PerDrawSlot   = 1;

} // namespace


namespace Copium
{

    RenderContext::RenderContext()
        : m_perCameraBuffer(GraphicsBufferDesc::Constant(1, k_PerCameraSizeInBytes))
        , m_perDrawBuffers(GraphicsBufferDesc::Constant(k_PerDrawElements, k_PerDrawSizeInBytes))
    {
    }


    void RenderContext::SetRenderData(RenderData&& renderData)
    {
        m_renderData = std::move(renderData);

        //- Update PerCamera constant buffer
        {
            std::span<uint8> perCameraBufferData = m_perCameraBuffer.Map();
            std::memcpy(perCameraBufferData.data(), &m_renderData.Camera, k_PerCameraSizeInBytes);
            m_perCameraBuffer.Unmap();
        }
        //- Update PerDraw constant buffers
        {
            const uint32 entitiesCount = static_cast<uint32>(m_renderData.Entities.size());

            COP_ASSERT(entitiesCount <= k_PerDrawElements);

            PerDraw perDraw;
            const uint32 perDrawElementSizeInBytes = m_perDrawBuffers.GetElementSize();

            std::span<uint8> perDrawBufferData = m_perDrawBuffers.Map();
            uint8* perDrawBufferDataPtr = perDrawBufferData.data();

            for (uint32 i = 0; i < entitiesCount; i++)
            {
                perDraw._LocalToWorld = m_renderData.Entities[i].LocalToWorld;

                std::memcpy(perDrawBufferDataPtr, &perDraw, k_PerDrawSizeInBytes);

                perDrawBufferDataPtr += perDrawElementSizeInBytes;
            }

            m_perDrawBuffers.Unmap();
        }
    }

    void RenderContext::NewFrame()
    {
        m_commandBuffer.BindConstantBuffer(&m_perCameraBuffer, k_PerCameraSlot);
    }


    void RenderContext::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        m_commandBuffer.BeginRenderPass(renderPassHandle);
    }

    void RenderContext::DrawEntities()
    {
        TextureHandle currentBaseColorMap = TextureHandle::Invalid;
        TextureHandle currentNormalMap = TextureHandle::Invalid;

        const uint32 entitiesCount = static_cast<uint32>(m_renderData.Entities.size());

        for (uint32 i = 0; i < entitiesCount; i++)
        {
            const EntityRenderData& entityRenderData = m_renderData.Entities[i];

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
            m_commandBuffer.BindConstantBuffer(&m_perDrawBuffers, k_PerDrawSlot, i);
            m_commandBuffer.DrawMesh(mesh);
        }
    }

} // namespace Copium
