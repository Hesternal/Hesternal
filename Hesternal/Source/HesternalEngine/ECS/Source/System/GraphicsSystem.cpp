module;

#include "Hesternal/Core/Debug.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.System.Graphics;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.ECS.Components;
import HesternalEngine.Graphics;
import HesternalEngine.Graphics.RenderData;

import <unordered_map>;
import <utility>;


namespace
{
    using namespace Hesternal;


    // NOTE(v.matushkin): Probably should move this functions to something like LightUtils or Math::Light

    [[nodiscard]] static constexpr float32 DistanceAttenuationScale(float32 lightRange) noexcept
    {
        return 1.0f / (lightRange * lightRange);
    }

    /// <returns>{angleAttenuationScale, angleAttenuationOffset}</returns>
    [[nodiscard]] static std::pair<float32, float32> AngleAttenuation(float32 spotAngle, float32 innerSpotPercent) noexcept
    {
        const float32 innerSpotAngle = spotAngle * (innerSpotPercent / 100.0f);

        // NOTE(v.matushkin): Should I clamp these or at least use abs?
        const float32 cosOuterConeHalfAngle = Math::Cos(Math::ToRadians(spotAngle * 0.5f));
        const float32 cosInnerConeHalfAngle = Math::Cos(Math::ToRadians(innerSpotAngle * 0.5f));

        const float32 angleAttenuationScale = 1.0f / Math::Max(0.0001f, cosInnerConeHalfAngle - cosOuterConeHalfAngle);
        const float32 angleAttenuationOffset = -cosOuterConeHalfAngle * angleAttenuationScale;

        return { angleAttenuationScale, angleAttenuationOffset };
    }

} // namespace


namespace Hesternal
{

    void GraphicsSystem::OnCreate(EntityManager& entityManager)
    {
        HS_UNUSED(entityManager);
    }

    void GraphicsSystem::OnDestroy(EntityManager& entityManager)
    {
        HS_UNUSED(entityManager);
    }

    void GraphicsSystem::OnUpdate(EntityManager& entityManager)
    {
        RenderData renderData;

        //- Get Camera
        {
            const auto cameraView = entityManager.GetView<const Transform, const Camera>();
            HS_ASSERT_MSG(cameraView.size_hint() == 1, "The scene must have exactly 1 camera");
            const auto [cameraEntity, cameraTransform, camera] = *cameraView.each().begin();

            renderData.Camera = CameraRenderData{
                .View           = camera.View,
                .Projection     = camera.Projection,
                .ViewProjection = camera.ViewProjection,
                .Position       = cameraTransform.Position,
            };
        }

        //- Get DirectionalLights
        {
            const auto directionalLightView = entityManager.GetView<const Transform, const DirectionalLight>();

            for (const auto [directionalLightEntity, directionalLightTransform, directionalLight] : directionalLightView.each())
            {
                renderData.DirectionalLights.push_back(DirectionalLightRenderData{
                        .Forward = Quaternion::Mul(directionalLightTransform.Rotation, Math::Forward()),
                        .Color   = Float3(directionalLight.Color),
                    });
            }
        }
        //- Get PointLights
        {
            const auto pointLightView = entityManager.GetView<const Transform, const PointLight>();

            for (const auto [pointLightEntity, pointLightTransform, pointLight] : pointLightView.each())
            {
                renderData.PointLights.push_back(PointLightRenderData{
                        .Position                 = pointLightTransform.Position,
                        .Color                    = Float3(pointLight.Color) * pointLight.Intensity,
                        .DistanceAttenuationScale = DistanceAttenuationScale(pointLight.Range),
                    });
            }
        }
        //- Get SpotLights
        {
            const auto spotLightView = entityManager.GetView<const Transform, const SpotLight>();

            for (const auto [spotLightEntity, spotLightTransform, spotLight] : spotLightView.each())
            {
                const auto [angleAttenuationScale, angleAttenuationOffset] = AngleAttenuation(spotLight.SpotAngle, spotLight.InnerSpotPercent);
                renderData.SpotLights.push_back(SpotLightRenderData{
                        .Position                 = spotLightTransform.Position,
                        .Forward                  = Quaternion::Mul(spotLightTransform.Rotation, Math::Forward()),
                        .Color                    = Float3(spotLight.Color) * spotLight.Intensity,
                        .DistanceAttenuationScale = DistanceAttenuationScale(spotLight.Range),
                        .AngleAttenuationScale    = angleAttenuationScale,
                        .AngleAttenuationOffset   = angleAttenuationOffset,
                    });
            }
        }

        const auto renderMeshView = entityManager.GetView<const LocalToWorld, const RenderMesh>();

        std::unordered_map<std::shared_ptr<Material>, uint32> materialToIndex;
        std::unordered_map<std::shared_ptr<Mesh>, uint32> meshToIndex;

        for (const auto [entity, localToWorld, renderMesh] : renderMeshView.each())
        {
            //- Get Material index
            uint32 materialIndex;

            if (const auto materialToIndexIt = materialToIndex.find(renderMesh.Material); materialToIndexIt != materialToIndex.end())
            {
                materialIndex = materialToIndexIt->second;
            }
            else
            {
                materialIndex = static_cast<uint32>(renderData.Materials.size());
                materialToIndex.emplace(renderMesh.Material, materialIndex);
                renderData.Materials.push_back(renderMesh.Material);
            }

            //- Get Mesh index
            uint32 meshIndex;

            if (const auto meshToIndexIt = meshToIndex.find(renderMesh.Mesh); meshToIndexIt != meshToIndex.end())
            {
                meshIndex = meshToIndexIt->second;
            }
            else
            {
                meshIndex = static_cast<uint32>(renderData.Meshes.size());
                meshToIndex.emplace(renderMesh.Mesh, meshIndex);
                renderData.Meshes.push_back(renderMesh.Mesh);
            }

            renderData.Entities.push_back(EntityRenderData{
                .LocalToWorld  = localToWorld.Value,
                .MaterialIndex = materialIndex,
                .MeshIndex     = meshIndex,
            });
        }

        Graphics::SetRenderData(std::move(renderData));
    }

} // namespace Hesternal
