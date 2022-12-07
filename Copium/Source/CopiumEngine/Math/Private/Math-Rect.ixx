export module CopiumEngine.Math:Rect;

import CopiumEngine.Core.CoreTypes;

import <type_traits>;


export namespace Copium
{

    template<typename T> requires std::is_same_v<int32, T> || std::is_same_v<float32, T>
    struct TRect final
    {
        T X;
        T Y;
        T Width;
        T Height;


        constexpr TRect() noexcept = default;
        constexpr TRect(const TRect& v) noexcept = default;

        constexpr TRect(T x, T y, T width, T height) noexcept
            : X(x)
            , Y(y)
            , Width(width)
            , Height(height)
        {
        }

        [[nodiscard]] constexpr T GetMaxX() const noexcept { return X + Width; }
        [[nodiscard]] constexpr T GetMaxY() const noexcept { return Y + Height; }

        [[nodiscard]] static constexpr TRect FromMinMax(T minX, T minY, T maxX, T maxY) noexcept
        {
            return TRect(minX, minY, maxX - minX, maxY - minY);
        }
    };


    // NOTE(v.matushkin): Not sure if I'm using extern correctly
    extern template struct TRect<int32>;
    extern template struct TRect<float32>;

    using RectInt = TRect<int32>;
    using Rect = TRect<float32>;

} // export namespace Copium
