export module HesternalEngine.ECS.Components:Editor;

import Hesternal.ECS;

import <string>;


// TODO(v.matushkin): <Components/Editor> This file shouldn't be in the HesternalEngine, but I need to use it in the Scene::AddModel


export namespace Hesternal
{

    struct EditorData final : IComponent
    {
        std::string EntityName;
    };

} // export namespace Hesternal
