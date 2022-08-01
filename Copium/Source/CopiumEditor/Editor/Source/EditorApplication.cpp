module;

#include "Engine/Core/Debug.hpp"

#include <cxxopts.hpp>

module CopiumEditor.EditorApplication;

import CopiumEditor.Assets.AssetDatabase;
import CopiumEngine.Assets.Shader;
import CopiumEngine.ECS.WorldManager;
import CopiumEngine.Graphics;

import <filesystem>;


std::unique_ptr<Copium::Application> Copium::CreateApplication(Copium::int32 argc, const char* const* argv)
{
    return std::make_unique<Copium::EditorApplication>(argc, argv);
}


namespace Copium
{

    EditorApplication::EditorApplication(int32 argc, const char* const* argv)
        : m_editorInitialized(false)
        , m_editorClosed(false)
    {
        // const std::string createProjectOption = "create-project";
        const std::string openProjectOption = "open-project";
        const std::string shaderDirOption = "shader-dir";

        cxxopts::Options cmdArguments("test", "A brief description");

        cmdArguments.add_options()
            // (createProjectOption, "Create an empty project at the given path", cxxopts::value<std::string>())
            (openProjectOption, "Open the project at the given path.", cxxopts::value<std::string>())
            (shaderDirOption, "Path to the Engine shader directory.", cxxopts::value<std::string>());

        cxxopts::ParseResult cmdResult = cmdArguments.parse(argc, argv);

        // bool usedCreateProjectOption = cmdResult.count(createProjectOption);
        bool usedOpenProjectOption = cmdResult.count(openProjectOption);

        COP_ASSERT_MSG(usedOpenProjectOption, "Option --open-project should be specified");
        COP_ASSERT_MSG(cmdResult.count(shaderDirOption), "Option --shader-dir should be specified");

        // Order of checks matters
        // COP_ASSERT(usedCreateProject || usedOpenProject,
        //            "Either '-{:s}' or '-{:s}' should be specified", argumentCreateProject, argumentOpenProject
        // );
        // COP_ASSERT(usedCreateProject != usedOpenProject,
        //            "Both '-{:s}' and '-{:s}' shouldn't be used at the same time",argumentCreateProject, argumentOpenProject
        // );

        m_projectPath = cmdResult[openProjectOption].as<std::string>();
        m_shaderDirPath = cmdResult[shaderDirOption].as<std::string>();

        // if (usedCreateProject)
        // {
        //     _CreateProject();
        // }
        // _OpenProject();
    }

    EditorApplication::~EditorApplication()
    {
        ShutdownSystems();
    }


    void EditorApplication::InitSettings(EngineSettings& engineSettings)
    {
        COP_LOG_INFO("EditorApplication::InitSettings()");

        engineSettings.GraphicsApi = GraphicsApi::DirectX11;

        engineSettings.WindowTitle  = L"CopiumEngine";
        engineSettings.WindowWidth  = 1280;
        engineSettings.WindowHeight = 1000;
    }

    void EditorApplication::InitSystems()
    {
        AssetDatabase::Init(m_projectPath, m_shaderDirPath);

        // TODO(v.matushkin): It shouldn't be set in the Editor like this, but right now I have no Idea how else
        Graphics::SetDefaultShader(AssetDatabase::LoadAsset<Shader>("Main.shader"));

        const auto sponzaModel = AssetDatabase::LoadAsset<ModelScene>("Assets/Sponza/sponza.obj");
        WorldManager::GetDefaultWorld()->GetDefaultScene()->AddModel(sponzaModel.get());

        m_editorInitialized = true;
    }

    void EditorApplication::ShutdownSystems()
    {
        if (m_editorInitialized == false || m_editorClosed)
        {
            return;
        }

        AssetDatabase::Shutdown();

        m_editorClosed = true;
    }


    // void EditorApplication::_CreateProject()
    // {
    //     if (std::filesystem::is_directory(m_projectPath))
    //     {
    //         // TODO(v.matushkin): Error
    //         COP_LOG_CRITICAL("CreateProject: specified project path was not a path to a folder");
    //     }
    // 
    //     if (std::filesystem::create_directory(m_projectPath) == false)
    //     {
    //         // If directory was not created(it was already there), check that it is empty
    //         if (std::filesystem::is_empty(m_projectPath))
    //         {
    //             // TODO(v.matushkin): Error
    //             COP_LOG_CRITICAL("CreateProject: specified project path was not an empty folder");
    //         }
    //     }
    // 
    // 
    // }
    // 
    // void EditorApplication::_OpenProject()
    // {
    // 
    // }

} // namespace Copium
