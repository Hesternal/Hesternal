export module CopiumEngine.Event.Input;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Event.EventTypes;

import <utility>;


export namespace Copium
{

    class Input final
    {
    public:
        // static void Init();
        // static void Shutdown();

        // [[nodiscard]] static ButtonAction GetButton(MouseButton button)    { return m_mouseButtons[std::to_underlying(button)]; }
        // [[nodiscard]] static ButtonAction GetButton(KeyboardButton button) { return m_keyboardButtons[std::to_underlying(button)]; }

        // [[nodiscard]] static bool IsButtonUp(MouseButton button)    { return GetButton(button) == ButtonAction::Up; }
        // [[nodiscard]] static bool IsButtonUp(KeyboardButton button) { return GetButton(button) == ButtonAction::Up; }
        // [[nodiscard]] static bool IsButtonDown(MouseButton button)    { return GetButton(button) == ButtonAction::Down; }
        // [[nodiscard]] static bool IsButtonDown(KeyboardButton button) { return GetButton(button) == ButtonAction::Down; }
        [[nodiscard]] static bool IsButtonPressed(MouseButton button)    { return m_mouseButtons[std::to_underlying(button)]; }
        [[nodiscard]] static bool IsButtonPressed(KeyboardButton button) { return m_keyboardButtons[std::to_underlying(button)]; }

        [[nodiscard]] static Int2 GetMousePosition() { return m_mousePosition; }
        // returns either -1 or 1
        // [[nodiscard]] static int8 GetMouseWheelDelta() { return m_mouseWheelDelta; }

        static void OnMouseButton(MouseButtonEvent event) { m_mouseButtons[std::to_underlying(event.Button)] = event.Pressed; }
        static void OnMousePosition(MousePositionEvent event) { m_mousePosition = event.Position; }
        // static void OnMouseWheel(MouseWheelEvent event) { m_mouseWheelDelta = event.Delta; }
        static void OnKeyboard(KeyboardEvent event) { m_keyboardButtons[std::to_underlying(event.Button)] = event.Pressed; }

    private:
        static inline bool m_keyboardButtons[255];
        static inline bool m_mouseButtons[3];

        static inline Int2 m_mousePosition;
        // static inline int8 m_mouseWheelDelta;
    };

} // export namespace Copium
