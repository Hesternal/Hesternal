module CopiumEngine.ECS.WorldManager;

import CopiumEngine.ECS.System.Graphics;

import <vector>;
import <memory>;


// TODO(v.matushkin): Register systems statically


namespace Copium
{

    void WorldManager::Init()
    {
        std::vector<std::unique_ptr<ISystem>> systems;
        systems.push_back(std::make_unique<GraphicsSystem>());

        m_defaulWorld = std::make_unique<World>(std::move(systems));
    }

    void WorldManager::Shutdown()
    {
        m_defaulWorld.reset();
    }

} // namespace Copium
