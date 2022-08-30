module;

#include "Engine/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.Scene;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.EntityManager;
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Math;

import <queue>;
import <utility>;


namespace Copium
{

    Scene::Scene(std::string&& sceneName)
        : m_sceneName(std::move(sceneName))
    {
    }

    Scene::~Scene()
    {
        // TODO(v.matushkin): Destroy Entities
    }


    // TODO(v.matushkin): Entities hierarchy
    void Scene::AddModel(const ModelScene* modelScene)
    {
        // TODO(v.matushkin): Don't know how else I should make entities from scene, Scene serialization will be pain
        EntityManager& entityManager = WorldManager::GetDefaultWorld()->GetEntityManager();

        Float4x4 sponzaTransform = Float4x4::Scale(0.005f);

        std::queue<const ModelNode*> modelNodesQueue;
        modelNodesQueue.push(modelScene->RootNode.get());

        while (modelNodesQueue.empty() == false)
        {
            const ModelNode* parentNode = modelNodesQueue.front();
            modelNodesQueue.pop();

            Entity entity = entityManager.CreateEntity();

            if (parentNode->MeshIndices.empty() == false)
            {
                entityManager.AddComponent<LocalToWorld>(entity, LocalToWorld{ .Value = sponzaTransform });

                const ModelMesh& modelMesh = modelScene->Meshes[parentNode->MeshIndices[0]];

                entityManager.AddComponent<RenderMesh>(entity, RenderMesh{
                    .Mesh     = modelMesh.Mesh,
                    .Material = modelScene->Materials[modelMesh.MaterialIndex],
                });
            }

            for (const std::unique_ptr<ModelNode>& childNode : parentNode->Children)
            {
                modelNodesQueue.push(childNode.get());
            }
        }
    }

} // namespace Copium
