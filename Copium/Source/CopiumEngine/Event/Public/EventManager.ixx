export module CopiumEngine.Event.EventManager;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Event.EventTypes;
import CopiumEngine.Math;
import CopiumEngine.Platform.PlatformTypes;

import <functional>;
import <optional>;
import <unordered_map>;
import <vector>;


export namespace Copium
{

    class EventManager final
    {
    public:
        // template<typename TEvent>
        // using Callback = std::function<void(TEvent evt)>;

        using WindowCloseCallback  = std::function<void()>;
        using WindowResizeCallback = std::function<void(const WindowResizeEvent& evt)>;

    public:
        static void DispatchEvents();

        // template<typename TEvent>
        // static void RegisterCallback(Callback<TEvent> callback)
        // {
        //     // NOTE(v.matushkin): Just use always_false? https://artificial-mind.net/blog/2020/10/03/always-false
        //     static_assert(false && sizeof(TEvent*), "Not supported event type");
        // }

        // template<typename TEvent>
        // static void SendEvent(TEvent&& evt)
        // {
        //     // NOTE(v.matushkin): Just use always_false? https://artificial-mind.net/blog/2020/10/03/always-false
        //     static_assert(false && sizeof(TEvent*), "Not supported event type");
        // }


        static void RegisterWindowCloseCallback(WindowHandle windowHandle, WindowCloseCallback callback);
        static void RegisterWindowResizeCallback(WindowHandle windowHandle, WindowResizeCallback callback);

        static void SendWindowCloseEvent(WindowHandle windowHandle);
        static void SendWindowResizeEvent(WindowHandle windowHandle, WindowResizeEvent&& evt);

        static void SendMouseButtonEvent(MouseButtonEvent event);
        static void SendMousePositionEvent(MousePositionEvent event);
        static void SendKeyboardEvent(KeyboardEvent event);

    private:
        static inline std::unordered_map<WindowHandle, WindowCloseCallback>  m_windowCloseCallbacks;
        static inline std::unordered_map<WindowHandle, WindowResizeCallback> m_windowResizeCallbacks;

        static inline std::unordered_map<WindowHandle, std::optional<WindowResizeEvent>> m_windowResizeEvents;
    };

} // export namespace Copium
