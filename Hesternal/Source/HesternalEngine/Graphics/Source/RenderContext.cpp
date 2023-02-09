module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Graphics.RenderContext;

import Hesternal.Core;
import Hesternal.Math;


namespace
{
    using namespace Hesternal;


    struct PerFrame final
    {
        uint32  _NumDirectionalLights;
        uint32  _NumPointLights;
        uint32  _NumSpotLights;
        float32 __padding0_PerFrame__;
    };

    struct PerCamera final
    {
        Float4x4 _CameraView;
        Float4x4 _CameraProjection;
        Float4x4 _CameraViewProjection;
        Float3   _CameraPositionWS;
        float32  __padding0_PerCamera__;
    };

    // NOTE(v.matushkin): Will be padded to 256 bytes by GraphicsBuffer
    struct PerDraw final
    {
        Float4x4 _LocalToWorld;
    };


    struct DirectionalLightData
    {
        Float3  Forward;
        float32 __padding0__;
        Float3  Color;
        float32 __padding1__;
    };

    struct PointLightData
    {
        Float3  Position;
        float32 DistanceAttenuationScale;
        Float3  Color;
        float32 __padding0__;
    };

    struct SpotLightData
    {
        Float3  Position;
        float32 DistanceAttenuationScale;
        Float3  Forward;
        float32 AngleAttenuationScale;
        Float3  Color;
        float32 AngleAttenuationOffset;
    };


    static constexpr int32 k_PerFrameSizeInBytes  = sizeof(PerFrame);
    static constexpr int32 k_PerCameraSizeInBytes = sizeof(PerCamera);
    static constexpr int32 k_PerDrawSizeInBytes   = sizeof(PerDraw);
    // NOTE(v.matushkin): 1500 needed for Bistro, 2500 for SanMiguel
    static constexpr int32 k_PerDrawElements      = 2500;

    static constexpr int32 k_DirectionalLightSizeInBytes = sizeof(DirectionalLightData);
    static constexpr int32 k_PointLightSizeInBytes       = sizeof(PointLightData);
    static constexpr int32 k_SpotLightSizeInBytes        = sizeof(SpotLightData);

    static constexpr uint32 k_PerFrameSlot  = 0;
    static constexpr uint32 k_PerCameraSlot = 1;
    static constexpr uint32 k_PerDrawSlot   = 2;

    static constexpr uint32 k_DirectionalLightsSlot = 0;
    static constexpr uint32 k_PointLightsSlot       = 1;
    static constexpr uint32 k_SpotLightsSlot        = 2;

    static constexpr uint32 k_DirectionalLightsGrow = 1;
    static constexpr uint32 k_PointLightsGrow       = 5;
    static constexpr uint32 k_SpotLightsGrow        = 5;


    // NOTE(v.matushkin): Check alignment, should be done in GraphicsBuffer probably
    static_assert(k_PerFrameSizeInBytes % 16 == 0);
    static_assert(k_PerCameraSizeInBytes % 16 == 0);
    static_assert(k_PerDrawSizeInBytes % 16 == 0);
    static_assert(k_DirectionalLightSizeInBytes % 16 == 0);
    static_assert(k_PointLightSizeInBytes % 16 == 0);
    static_assert(k_SpotLightSizeInBytes % 16 == 0);

} // namespace


namespace Hesternal
{

    // NOTE(v.matushkin): It would probably be better to either create empty/invalid buffers at the start
    //   or to allocate them with some reasonable size, so there will be no need to resize them just after they were created.
    //   Of course it would be even better if there was a way to query the amount of lights on the loaded or to be loaded scenes.
    RenderContext::RenderContext()
        : m_perFrameBuffer(GraphicsBufferDesc::Constant(1, k_PerFrameSizeInBytes))
        , m_perCameraBuffer(GraphicsBufferDesc::Constant(1, k_PerCameraSizeInBytes))
        , m_perDrawBuffers(GraphicsBufferDesc::Constant(k_PerDrawElements, k_PerDrawSizeInBytes))
        , m_directionalLightsBuffer(GraphicsBufferDesc::Structured(k_DirectionalLightsGrow, k_DirectionalLightSizeInBytes))
        , m_pointLightsBuffer(GraphicsBufferDesc::Structured(k_PointLightsGrow, k_PointLightSizeInBytes))
        , m_spotLightsBuffer(GraphicsBufferDesc::Structured(k_SpotLightsGrow, k_SpotLightSizeInBytes))
    {}


    void RenderContext::NewFrame()
    {
        // NOTE(v.matushkin): Assuming I need to update everything every frame, otherwise it's a waste

        const uint32 numDirectionalLights = static_cast<uint32>(m_renderData.DirectionalLights.size());
        const uint32 numPointLights       = static_cast<uint32>(m_renderData.PointLights.size());
        const uint32 numSpotLights        = static_cast<uint32>(m_renderData.SpotLights.size());

        if (numDirectionalLights > m_directionalLightsBuffer.GetElementCount())
        {
            m_directionalLightsBuffer = GraphicsBuffer(GraphicsBufferDesc::Structured(numDirectionalLights + k_DirectionalLightsGrow, k_DirectionalLightSizeInBytes));
        }
        if (numPointLights > m_pointLightsBuffer.GetElementCount())
        {
            m_pointLightsBuffer = GraphicsBuffer(GraphicsBufferDesc::Structured(numPointLights + k_PointLightsGrow, k_PointLightSizeInBytes));
        }
        if (numSpotLights > m_spotLightsBuffer.GetElementCount())
        {
            m_spotLightsBuffer = GraphicsBuffer(GraphicsBufferDesc::Structured(numSpotLights + k_SpotLightsGrow, k_SpotLightSizeInBytes));
        }

        //- Update constant buffers
        //-- PerFrame
        {
            const PerFrame perFrame = {
                ._NumDirectionalLights = numDirectionalLights,
                ._NumPointLights       = numPointLights,
                ._NumSpotLights        = numSpotLights,
            };

            std::span<uint8> perFrameBufferData = m_commandBuffer.MapBuffer(m_perFrameBuffer);
            std::memcpy(perFrameBufferData.data(), &perFrame, k_PerFrameSizeInBytes);
            m_commandBuffer.UnmapBuffer(m_perFrameBuffer);
        }
        //-- PerCamera
        {
            const PerCamera perCamera = {
                ._CameraView           = m_renderData.Camera.View,
                ._CameraProjection     = m_renderData.Camera.Projection,
                ._CameraViewProjection = m_renderData.Camera.ViewProjection,
                ._CameraPositionWS     = m_renderData.Camera.Position,
            };

            std::span<uint8> perCameraBufferData = m_commandBuffer.MapBuffer(m_perCameraBuffer);
            std::memcpy(perCameraBufferData.data(), &perCamera, k_PerCameraSizeInBytes);
            m_commandBuffer.UnmapBuffer(m_perCameraBuffer);
        }
        //-- PerDraw
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

        m_commandBuffer.BindConstantBuffer(&m_perFrameBuffer, k_PerFrameSlot);
        m_commandBuffer.BindConstantBuffer(&m_perCameraBuffer, k_PerCameraSlot);

        // NOTE(v.matushkin): I don't think there is much sense in copying the same data just for the DirectX constant
        //   buffers layout, just make properly aligned structures in RenderData.

        //- Update lights structured buffers
        //-- Directional
        {
            DirectionalLightData lightData;
            lightData.__padding0__ = 0.0f;
            lightData.__padding1__ = 0.0f;

            std::span<uint8> lightsBufferData = m_commandBuffer.MapBuffer(m_directionalLightsBuffer);
            uint8* lightsBufferDataPtr = lightsBufferData.data();

            for (const DirectionalLightRenderData& lightRenderData : m_renderData.DirectionalLights)
            {
                lightData.Forward = lightRenderData.Forward;
                lightData.Color = lightRenderData.Color;

                std::memcpy(lightsBufferDataPtr, &lightData, k_DirectionalLightSizeInBytes);
                lightsBufferDataPtr += k_DirectionalLightSizeInBytes;
            }

            m_commandBuffer.UnmapBuffer(m_directionalLightsBuffer);
        }
        //-- Point
        {
            PointLightData lightData;
            lightData.__padding0__ = 0.0f;

            std::span<uint8> lightsBufferData = m_commandBuffer.MapBuffer(m_pointLightsBuffer);
            uint8* lightsBufferDataPtr = lightsBufferData.data();

            for (const PointLightRenderData& lightRenderData : m_renderData.PointLights)
            {
                lightData.Position = lightRenderData.Position;
                lightData.Color = lightRenderData.Color;
                lightData.DistanceAttenuationScale = lightRenderData.DistanceAttenuationScale;

                std::memcpy(lightsBufferDataPtr, &lightData, k_PointLightSizeInBytes);
                lightsBufferDataPtr += k_PointLightSizeInBytes;
            }

            m_commandBuffer.UnmapBuffer(m_pointLightsBuffer);
        }
        //-- Spot
        {
            SpotLightData lightData;

            std::span<uint8> lightsBufferData = m_commandBuffer.MapBuffer(m_spotLightsBuffer);
            uint8* lightsBufferDataPtr = lightsBufferData.data();

            for (const SpotLightRenderData& lightRenderData : m_renderData.SpotLights)
            {
                lightData.Position = lightRenderData.Position;
                lightData.Forward = lightRenderData.Forward;
                lightData.Color = lightRenderData.Color;
                lightData.DistanceAttenuationScale = lightRenderData.DistanceAttenuationScale;
                lightData.AngleAttenuationScale = lightRenderData.AngleAttenuationScale;
                lightData.AngleAttenuationOffset = lightRenderData.AngleAttenuationOffset;

                std::memcpy(lightsBufferDataPtr, &lightData, k_SpotLightSizeInBytes);
                lightsBufferDataPtr += k_SpotLightSizeInBytes;
            }

            m_commandBuffer.UnmapBuffer(m_spotLightsBuffer);
        }

        m_commandBuffer.BindStructuredBuffer(&m_directionalLightsBuffer, k_DirectionalLightsSlot);
        m_commandBuffer.BindStructuredBuffer(&m_pointLightsBuffer, k_PointLightsSlot);
        m_commandBuffer.BindStructuredBuffer(&m_spotLightsBuffer, k_SpotLightsSlot);
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
