module;

#include "Copium/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.Scene;

import Copium.Core;
import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.EntityManager;
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Math;

import <stack>;
import <utility>;


namespace
{
    using namespace Copium;


    static constexpr float32 k_SponzaScale = 0.005f;

} // namespace


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


    // TODO(v.matushkin): Code is shit
    void Scene::AddModel(const ModelScene* modelScene)
    {
        // TODO(v.matushkin): Don't know how else I should make entities from scene, Scene serialization will be pain
        EntityManager& entityManager = WorldManager::GetDefaultWorld()->GetEntityManager();

        struct HierarchyNode
        {
            const ModelNode*     ModelNode;
            std::vector<Entity>& EntityChildren;
            int32                NextChildIndex;
            Entity               Entity;
        };

        std::stack<HierarchyNode> modelNodesStack;
        const std::vector<ModelMesh>& modelMeshes = modelScene->Meshes;

        auto createEntity = [modelScene, &entityManager, &modelMeshes](ModelNode* modelNode) -> Entity
        {
            Entity entity = entityManager.CreateEntity();

            if (modelNode->MeshIndices.empty() == false)
            {
                entityManager.AddComponent<Translation>(entity, Translation{ .Value = Float3::Zero() });
                entityManager.AddComponent<Rotation>(entity, Rotation{ .Value = Quaternion::Identity() });
                entityManager.AddComponent<Scale>(entity, Scale{ .Value = k_SponzaScale });
                entityManager.AddComponent<LocalToWorld>(entity, LocalToWorld{ .Value = Float4x4::Identity() });

                const ModelMesh& modelMesh = modelMeshes[modelNode->MeshIndices[0]];

                entityManager.AddComponent<RenderMesh>(entity, RenderMesh{
                    .Mesh     = modelMesh.Mesh,
                    .Material = modelScene->Materials[modelMesh.MaterialIndex],
                });
            }

            return entity;
        };

        {
            ModelNode* const rootModelNode = modelScene->RootNode.get();

            Entity rootEntity = createEntity(rootModelNode);
            m_rootEntities.push_back(rootEntity);

            if (rootModelNode->Children.empty() == false)
            {
                modelNodesStack.push(HierarchyNode{
                    .ModelNode      = rootModelNode,
                    .EntityChildren = entityManager.AddComponent<Child>(rootEntity, Child{}).Children,
                    .NextChildIndex = 0,
                    .Entity         = rootEntity,
                });
            }
        }

        while (modelNodesStack.empty() == false)
        {
            HierarchyNode& parentNode = modelNodesStack.top();
            const auto& childrenModelNodes = parentNode.ModelNode->Children;

            bool popParentNode = true;

            while (parentNode.NextChildIndex < childrenModelNodes.size())
            {
                ModelNode* const childModelNode = childrenModelNodes[parentNode.NextChildIndex++].get();

                Entity childEntity = createEntity(childModelNode);

                parentNode.EntityChildren.push_back(childEntity);
                entityManager.AddComponent<Parent>(childEntity, Parent{ .Value = parentNode.Entity });

                if (childModelNode->Children.empty() == false)
                {
                    modelNodesStack.push(HierarchyNode{
                        .ModelNode      = childModelNode,
                        .EntityChildren = entityManager.AddComponent<Child>(childEntity, Child{}).Children,
                        .NextChildIndex = 0,
                        .Entity         = childEntity,
                    });

                    popParentNode = false;
                    break;
                }
            }

            if (popParentNode)
            {
                modelNodesStack.pop();
            }
        }
    }

} // namespace Copium
