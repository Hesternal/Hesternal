export module CopiumEngine.ECS.Components:Editor;

import :IComponent;

import <string>;


// TODO(v.matushkin): <Components/Editor> This file shouldn't be in the CopiumEngine, but I need to use it in the Scene::AddModel


export namespace Copium
{

    struct EditorData final : IComponent
    {
        std::string EntityName;
    };

} // export namespace Copium
