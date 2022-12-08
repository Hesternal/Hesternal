export module CopiumEngine.Event.EventTypes;

import Copium.Core;

import CopiumEngine.Math;


export namespace Copium
{

    struct WindowResizeEvent
    {
        uint16 Width;
        uint16 Height;
    };


    // enum ButtonAction : uint8
    // {
    //     Up     = 0,
    //     Down   = 1,
    //     Repeat = 2,
    // };

    enum class MouseButton : uint8
    {
        Left   = 0,
        Right  = 1,
        Middle = 2,
    };

    enum class KeyboardButton : uint16
    {
        LeftShift = 16,

        Escape    = 27,
        Space     = 32,

        A         = 65,
        B         = 66,
        C         = 67,
        D         = 68,
        E         = 69,
        F         = 70,
        G         = 71,
        H         = 72,
        I         = 73,
        J         = 74,
        K         = 75,
        L         = 76,
        M         = 77,
        N         = 78,
        O         = 79,
        P         = 80,
        Q         = 81,
        R         = 82,
        S         = 83,
        T         = 84,
        U         = 85,
        V         = 86,
        W         = 87,
        X         = 88,
        Y         = 89,
        Z         = 90,
    };


    struct MouseButtonEvent final
    {
        MouseButton Button;
        bool        Pressed;
    };

    struct MousePositionEvent final
    {
        Int2 Position;
    };

    // struct MouseWheelEvent final
    // {
    //     Float2 Delta;
    // };

    struct KeyboardEvent final
    {
        KeyboardButton Button;
        bool           Pressed;
    };

}
