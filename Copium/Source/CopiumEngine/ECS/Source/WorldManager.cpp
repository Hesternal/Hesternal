module CopiumEngine.ECS.WorldManager;

import CopiumEngine.ECS.System.Input;
import CopiumEngine.ECS.System.Graphics;
import CopiumEngine.ECS.System.Transform;

import <vector>;
import <memory>;


// TODO(v.matushkin): Register systems statically


namespace Copium
{

    void WorldManager::Init()
    {
        std::vector<std::unique_ptr<ISystem>> systems;
        systems.push_back(std::make_unique<InputSystem>());
        systems.push_back(std::make_unique<TransformSystem>());
        systems.push_back(std::make_unique<GraphicsSystem>());

        m_defaulWorld = std::make_unique<World>(std::move(systems));
    }

    void WorldManager::Shutdown()
    {
        m_defaulWorld.reset();
    }

} // namespace Copium
