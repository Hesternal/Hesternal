export module CopiumEngine.ImGui.ImGuiContext;

import Copium.Core;
import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Graphics.CommandBuffer;
import CopiumEngine.Graphics.GraphicsBuffer;
import CopiumEngine.Platform.PlatformTypes;

import <memory>;

// TODO(v.matushkin): There should be only one ImGui context created from what I understand.
//  But right now ImGuiContext creation is not controlled in any way


export namespace Copium
{

    // NOTE(v.matushkin): Need to distinguish handle types between Texture/RenderTexture for IGraphicsDevice
    union ImGuiTexture
    {
        struct
        {
            uint32 Handle;
            bool   IsRenderTexture;
        } Parts;
        uint64 Whole;
    };


    // TODO(v.matushkin): Should rename because ImGui also has ImGuiContext struct
    class ImGuiContext final
    {
    public:
        static void Init(WindowHandle windowHandle, std::shared_ptr<Shader>&& imguiShader);
        static void Shutdown();

        static void BeginUpdate();
        static void EndUpdate();
        static void Render(CommandBuffer& commandBuffer);

    private:
        static void _Renderer_Init();
        static void _Renderer_InitPlatformInterface();
        static void _Renderer_Shutdown();
        static void _Renderer_DrawData(CommandBuffer& commandBuffer);

    private:
        static inline std::shared_ptr<Shader>         m_imguiShader;
        static inline std::unique_ptr<Texture>        m_imguiFontTexture;
        static inline std::unique_ptr<GraphicsBuffer> m_vertexBuffer;
        static inline std::unique_ptr<GraphicsBuffer> m_indexBuffer;
        static inline std::unique_ptr<GraphicsBuffer> m_cameraBuffer;
    };

} // export namespace Copium
