module;

#include "Hesternal/Core/Debug.hpp"

#include <cxxopts.hpp>

module HesternalEditor.EditorApplication;

import HesternalEditor.Assets.AssetDatabase;
import HesternalEngine.Assets.Shader;
import HesternalEngine.ECS.WorldManager;
import HesternalEngine.Engine.EngineSettings;
import HesternalEngine.Graphics;
import HesternalEngine.Graphics.EngineRenderPass;
import HesternalEngine.Graphics.RenderGraph;
import HesternalEngine.ImGui.ImGuiContext;

import HesternalEditor.Editor.EditorSettings;
import HesternalEditor.Editor.ImGuiRenderPass;
import HesternalEditor.Editor.InspectorWindow;
import HesternalEditor.Editor.HierarchyWindow;
import HesternalEditor.Editor.RenderStatsWindow;
import HesternalEditor.Editor.SceneWindow;

import <filesystem>;


namespace
{
    using namespace Hesternal;


    static EditorApplication s_EditorApplication;

} // namespace


namespace Hesternal
{

    Application* Application::Get()
    {
        return &s_EditorApplication;
    }


    EditorApplication::EditorApplication()
        : m_editorInitialized(false)
        , m_editorClosed(false)
    {
    }

    EditorApplication::~EditorApplication()
    {
        OnEngine_Shutdown();
    }


    void EditorApplication::OnEngine_Init(int32 argc, const char* const* argv)
    {
        if (m_editorInitialized)
        {
            return;
        }

        m_editorInitialized = true;
        m_editorClosed = false;

        HS_LOG_TRACE("EditorApplication Init");

        // const std::string createProjectOption = "create-project";
        const std::string openProjectOption = "open-project";
        const std::string shaderDirOption = "shader-dir";

        cxxopts::Options cmdArguments("test", "A brief description");

        cmdArguments.add_options()
            // (createProjectOption, "Create an empty project at the given path", cxxopts::value<std::string>())
            (openProjectOption, "Open the project at the given path.", cxxopts::value<std::string>())
            (shaderDirOption, "Path to the Engine shader directory.", cxxopts::value<std::string>());

        cxxopts::ParseResult cmdResult = cmdArguments.parse(argc, argv);

        // NOTE(v.matushkin): Useless method calls without ASSERT
        // [[maybe_unused]] const bool usedCreateProjectOption = cmdResult.count(createProjectOption);
        [[maybe_unused]] const bool usedOpenProjectOption = cmdResult.count(openProjectOption);

        HS_ASSERT_MSG(usedOpenProjectOption, "Option --open-project should be specified");
        HS_ASSERT_MSG(cmdResult.count(shaderDirOption), "Option --shader-dir should be specified");

        // Order of checks matters
        // HS_ASSERT(usedCreateProject || usedOpenProject,
        //            "Either '-{:s}' or '-{:s}' should be specified", argumentCreateProject, argumentOpenProject
        // );
        // HS_ASSERT(usedCreateProject != usedOpenProject,
        //            "Both '-{:s}' and '-{:s}' shouldn't be used at the same time",argumentCreateProject, argumentOpenProject
        // );

        m_projectPath = cmdResult[openProjectOption].as<std::string>();
        m_shaderDirPath = cmdResult[shaderDirOption].as<std::string>();

        // if (usedCreateProject) 
        // {
        //     _CreateProject();
        // }
        // _OpenProject();


        EngineSettings& engineSettings = EngineSettings::Get();
        engineSettings.RenderWidth = 1280;
        engineSettings.RenderHeight = 1000;
        engineSettings.GraphicsApi = GraphicsApi::DirectX11;

        EditorSettings& editorSettings = EditorSettings::Get();
        editorSettings.WindowTitle = L"HesternalEngine";
        editorSettings.WindowWidth = 1920;
        editorSettings.WindowHeight = 1080;
    }

    void EditorApplication::OnEngine_SystemsInit()
    {
        HS_LOG_TRACE("EditorApplication SystemsInit"); 

        {
            EditorSettings& editorSettings = EditorSettings::Get();
            WindowDesc windowDesc = {
                .Title  = editorSettings.WindowTitle,
                .Width  = editorSettings.WindowWidth,
                .Height = editorSettings.WindowHeight,
            };
            m_mainWindow = std::make_unique<Window>(std::move(windowDesc));
        }

        AssetDatabase::Init(m_projectPath, m_shaderDirPath);

        // TODO(v.matushkin): It shouldn't be set in the Editor like this, but right now I have no idea how else
        Graphics::SetDefaultShader(AssetDatabase::LoadAsset<Shader>("SimpleLit.shader"));
        ImGuiContext::Init(m_mainWindow->GetWindowHandle(), AssetDatabase::LoadAsset<Shader>("ImGui.shader"));

        const auto sponzaModel = AssetDatabase::LoadAsset<ModelScene>("Assets/Sponza/sponza.obj");
        WorldManager::GetDefaultWorld()->GetDefaultScene()->AddModel(sponzaModel.get());

        auto engineRenderPass = std::make_unique<EngineRenderPass>();
        auto* engineRenderPassPtr = engineRenderPass.get();

        m_renderGraph = std::make_unique<RenderGraph>(m_mainWindow->GetSwapchainHandle());
        m_renderGraph->AddRenderPass(std::move(engineRenderPass));
        m_renderGraph->AddRenderPass(std::make_unique<ImGuiRenderPass>());
        m_renderGraph->Build(Graphics::GetGraphicsDevice());

        m_editorWindows.push_back(std::make_unique<SceneWindow>(engineRenderPassPtr->GetEngineOutput()));
        m_editorWindows.push_back(std::make_unique<HierarchyWindow>());
        m_editorWindows.push_back(std::make_unique<InspectorWindow>());
        m_editorWindows.push_back(std::make_unique<RenderStatsWindow>());
    }

    void EditorApplication::OnEngine_Shutdown()
    {
        if (m_editorInitialized == false || m_editorClosed)
        {
            return;
        }

        m_editorInitialized = false;
        m_editorClosed = true;

        m_editorWindows.clear();

        m_renderGraph.reset();

        ImGuiContext::Shutdown();
        AssetDatabase::Shutdown();

        m_mainWindow.reset();

        HS_LOG_TRACE("EditorApplication Shutdown");
    }

    void EditorApplication::OnEngine_Update()
    {
        if (m_mainWindow->IsClosing())
        {
            Close();
            return;
        }

        ImGuiContext::BeginUpdate();

        for (std::unique_ptr<EditorWindow>& editorWindow : m_editorWindows)
        {
            editorWindow->Draw();
        }

        ImGuiContext::EndUpdate();
    }

    void EditorApplication::OnEngine_Render(RenderContext& renderContext)
    {
        m_renderGraph->Execute(renderContext);
    }


    // void EditorApplication::_CreateProject()
    // {
    //     if (std::filesystem::is_directory(m_projectPath))
    //     {
    //         // TODO(v.matushkin): Error
    //         HS_LOG_CRITICAL("CreateProject: specified project path was not a path to a folder");
    //     }
    // 
    //     if (std::filesystem::create_directory(m_projectPath) == false)
    //     {
    //         // If directory was not created(it was already there), check that it is empty
    //         if (std::filesystem::is_empty(m_projectPath))
    //         {
    //             // TODO(v.matushkin): Error
    //             HS_LOG_CRITICAL("CreateProject: specified project path was not an empty folder");
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

} // namespace Hesternal
