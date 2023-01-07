module;

#include "Hesternal/Core/Defines.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

#include <imgui.h>
#include <fmt/format.h>

module HesternalEditor.Editor.HierarchyWindow;

import Hesternal.Core;
import Hesternal.ECS;

import HesternalEngine.ECS.Components;
import HesternalEngine.ECS.World;
import HesternalEngine.ECS.WorldManager;

import HesternalEditor.GUI.Selection;

import <stack>;
import <utility>;
import <vector>;


namespace
{
    using namespace Hesternal;


    static constexpr ImGuiTreeNodeFlags k_TreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;


//    static void DoEntitiesGui(EntityManager& entityManager, const std::vector<Entity>& rootEntities)
//    {
//        struct EntityNode final
//        {
//            const std::vector<Entity>& Children;
//            int32                      NextChildIndex;
//        };
//
//        if (rootEntities.empty())
//        {
//            return;
//        }
//
//        std::stack<EntityNode> entityStack;
//        entityStack.push(EntityNode{ .Children = rootEntities, .NextChildIndex = 0 });
//
//        while (entityStack.empty() == false)
//        {
//            EntityNode& entityNode = entityStack.top();
//
//            bool popNode = true;
//
//            while (entityNode.NextChildIndex < entityNode.Children.size())
//            {
//                const Entity childEntity = entityNode.Children[entityNode.NextChildIndex++];
//                const bool isSelected = Selection::IsSelected(childEntity);
//
//                if (ImGui::Selectable(fmt::format("Entity {{{:d}}}", std::to_underlying(childEntity)).c_str(), isSelected))
//                {
//                    Selection::SetSelection(childEntity);
//                }
//
//                if (Child* entityChildren = entityManager.TryGetComponent<Child>(childEntity); entityChildren != nullptr)
//                {
//                    entityStack.push(EntityNode{ .Children = entityChildren->Children, .NextChildIndex = 0 });
//                    popNode = false;
//                    break;
//                }
//            }
//
//            if (popNode)
//            {
//                entityStack.pop();
//            }
//        }
//    }

    static void DoEntitiesGuiRecursive(EntityManager& entityManager, const std::vector<Entity>& entities)
    {
        for (const Entity entity : entities)
        {
            const EditorData* const entityEditorData = entityManager.TryGetComponent<EditorData>(entity);
            const std::string& entityName = entityEditorData != nullptr
                                          ? entityEditorData->EntityName
                                          : fmt::format("Entity {{{:d}}}", std::to_underlying(entity));

            const Child* const entityChildren = entityManager.TryGetComponent<Child>(entity);

            const bool isSelected = Selection::IsSelected(entity);

            if (entityChildren == nullptr)
            {
                if (ImGui::Selectable(entityName.c_str(), isSelected))
                {
                    Selection::SetSelection(entity);
                }
            }
            else
            {
                ImGuiTreeNodeFlags flags = k_TreeNodeFlags;
                if (isSelected)
                {
                    flags |= ImGuiTreeNodeFlags_Selected;
                }

                const bool isNodeOpen = ImGui::TreeNodeEx(entityName.c_str(), flags);

                if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
                {
                    Selection::SetSelection(entity);
                }
                if (isNodeOpen)
                {
                    DoEntitiesGuiRecursive(entityManager, entityChildren->Children);
                    ImGui::TreePop();
                }
            }
        }
    }

} // namespace


namespace Hesternal
{

    HierarchyWindow::HierarchyWindow()
        : EditorWindow("Hierarchy")
    {
    }


    void HierarchyWindow::OnGui()
    {
        World* const world = WorldManager::GetDefaultWorld();
        Scene* const scene = world->GetDefaultScene();
        EntityManager& entityManager = world->GetEntityManager();

        if (ImGui::TreeNodeEx(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            DoEntitiesGuiRecursive(entityManager, scene->GetRootEntities());

            ImGui::TreePop();
        }
    }

} // namespace Hesternal
