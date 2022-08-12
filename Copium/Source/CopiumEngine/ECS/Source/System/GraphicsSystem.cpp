module;

#include "Engine/Core/Debug.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Graphics;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.ECS.Components;
import CopiumEngine.Graphics;
import CopiumEngine.Graphics.RenderData;

import <unordered_map>;


namespace Copium
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
        const auto cameraView = entityManager.GetView<const LocalToWorld, const Camera>();
        COP_ASSERT_MSG(cameraView.size_hint() == 1, "The scene must have at least and only 1 camera");
        const auto [cameraEntity, cameraLocalToWorld, camera] = *cameraView.each().begin();

        const auto renderMeshView = entityManager.GetView<const LocalToWorld, const RenderMesh>();

        std::unordered_map<std::shared_ptr<Material>, uint32> materialToIndex;
        std::unordered_map<MeshHandle, uint32> meshToIndex;

        RenderData renderData;
        renderData.Camera = {
            .LocalToWorld = cameraLocalToWorld.Value,
            .Projection   = camera.Projection,
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
            const MeshHandle meshHandle = renderMesh.Mesh->GetHandle();
            uint32 meshIndex;

            if (const auto meshToIndexIt = meshToIndex.find(meshHandle); meshToIndexIt != meshToIndex.end())
            {
                meshIndex = meshToIndexIt->second;
            }
            else
            {
                meshIndex = static_cast<uint32>(renderData.Meshes.size());
                meshToIndex.emplace(meshHandle, meshIndex);
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

} // namespace Copium
