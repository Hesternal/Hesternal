module HesternalEngine.ECS.WorldManager;

import HesternalEngine.ECS.System.Camera;
import HesternalEngine.ECS.System.Input;
import HesternalEngine.ECS.System.Graphics;
import HesternalEngine.ECS.System.TransformToMatrix;
import HesternalEngine.ECS.System.TransformHierarchy;

import <memory>;
import <vector>;


// TODO(v.matushkin): Register systems statically


namespace Hesternal
{

    void WorldManager::Init()
    {
        std::vector<std::unique_ptr<ISystem>> systems;
        systems.push_back(std::make_unique<InputSystem>());
        systems.push_back(std::make_unique<TransformToMatrixSystem>());
        systems.push_back(std::make_unique<TransformHierarchySystem>());
        systems.push_back(std::make_unique<CameraSystem>());
        systems.push_back(std::make_unique<GraphicsSystem>());

        m_defaulWorld = std::make_unique<World>(std::move(systems));
    }

    void WorldManager::Shutdown()
    {
        m_defaulWorld.reset();
    }

} // namespace Hesternal
