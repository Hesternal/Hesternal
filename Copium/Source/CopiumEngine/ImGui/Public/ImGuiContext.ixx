export module CopiumEngine.ImGui.ImGuiContext;

import CopiumEngine.Platform.PlatformTypes;


// TODO(v.matushkin): There should be only one ImGui context created from what I understand.
//  But right now ImGuiContext creation is not controlled in any way


export namespace Copium
{

    class ImGuiContext final
    {
    public:
        static void Init(WindowHandle windowHandle);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
    };

} // export namespace Copium
