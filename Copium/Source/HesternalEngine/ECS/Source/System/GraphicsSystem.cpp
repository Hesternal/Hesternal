module;

#include "Hesternal/Core/Debug.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.System.Graphics;

import Hesternal.Core;

import HesternalEngine.ECS.Components;
import HesternalEngine.Graphics;
import HesternalEngine.Graphics.RenderData;

import <unordered_map>;


namespace Hesternal
{

    void GraphicsSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void GraphicsSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void GraphicsSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto cameraView = entityManager.GetView<const Camera>();
        COP_ASSERT_MSG(cameraView.size() == 1, "The scene must have exactly 1 camera");
        const auto [cameraEntity, camera] = *cameraView.each().begin();

        const auto renderMeshView = entityManager.GetView<const LocalToWorld, const RenderMesh>();

        std::unordered_map<std::shared_ptr<Material>, uint32> materialToIndex;
        std::unordered_map<std::shared_ptr<Mesh>, uint32> meshToIndex;

        RenderData renderData;
        renderData.Camera = {
            .View           = camera.View,
            .Projection     = camera.Projection,
            .ViewProjection = camera.ViewProjection,
        };

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
