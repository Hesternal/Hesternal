module;

#include "Copium/Core/Defines.hpp"
#include "Copium/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.Scene;

import Hesternal.Core;
import Hesternal.ECS;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;
import HesternalEngine.ECS.Components;
import HesternalEngine.ECS.WorldManager;

import <stack>;
import <utility>;


namespace Hesternal
{

    Scene::Scene(std::string&& sceneName)
        : m_sceneName(std::move(sceneName))
    {}

    Scene::~Scene()
    {
        // TODO(v.matushkin): Destroy Entities
    }


    // TODO(v.matushkin): Code is shit
    void Scene::AddModel(const ModelScene* modelScene)
    {
        // TODO(v.matushkin): Don't know how else I should make entities from scene, Scene serialization will be pain
        EntityManager& entityManager = WorldManager::GetDefaultWorld()->GetEntityManager();

        struct HierarchyNode final
        {
            const ModelNode*     ModelNode;
            std::vector<Entity>& EntityChildren;
            int32                NextChildIndex;
            Entity               Entity;
        };

        std::stack<HierarchyNode> modelNodesStack;
        const std::vector<ModelMesh>& modelMeshes = modelScene->Meshes;

        const auto CreateEntity = [modelScene, &entityManager, &modelMeshes](const ModelNode* const modelNode) -> Entity
        {
            Entity entity = entityManager.CreateEntity();
            entityManager.AddComponent<Transform>(entity,
                Transform::FromPositionRotationScale(modelNode->Position, modelNode->Rotation, modelNode->Scale)
            );
            entityManager.AddComponent<LocalToWorld>(entity, LocalToWorld{ .Value = Float4x4::Identity() });
            // TODO(v.matushkin): <Components/Editor>
            //   Right now this is the simplest way to add Editor components, probably in the future
            //   there should be multiple layers of components initialization so I can hook to it from the Editor.
            //   Or something like that, idk.
            // NOTE(v.matushkin): Copies of Node name strings, may be just move them
            entityManager.AddComponent<EditorData>(entity, EditorData{ .EntityName = modelNode->Name });

            if (modelNode->MeshIndex.has_value())
            {
                const ModelMesh& modelMesh = modelMeshes[*modelNode->MeshIndex];
                entityManager.AddComponent<RenderMesh>(entity, RenderMesh{
                    .Mesh     = modelMesh.Mesh,
                    // TODO(v.matushkin): Hack, using only first material for all submeshes
                    .Material = modelScene->Materials[modelMesh.MaterialIndices[0]],
                });
            }

            return entity;
        };

        {
            const ModelNode* const rootModelNode = modelScene->RootNode.get();

            Entity rootEntity = CreateEntity(rootModelNode);
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

                Entity childEntity = CreateEntity(childModelNode);

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

} // namespace Hesternal
