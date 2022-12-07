export module CopiumEngine.Math:Int2;

import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    struct Int2 final
    {
        // using BaseType = int32;


        int32 X;
        int32 Y;


        constexpr Int2() noexcept = default;
        constexpr Int2(const Int2& other) noexcept = default;

        explicit constexpr Int2(int32 scalar) noexcept : X(scalar), Y(scalar) {}

        constexpr Int2(int32 x, int32 y) noexcept : X(x), Y(y) {}

        //- Compound
        //-- Vector | Scalar
        constexpr Int2& operator+=(int32 rhs) noexcept { X += rhs; Y += rhs; return *this; }
        constexpr Int2& operator-=(int32 rhs) noexcept { X -= rhs; Y -= rhs; return *this; }
        constexpr Int2& operator*=(int32 rhs) noexcept { X *= rhs; Y *= rhs; return *this; }
        //-- Vector | Vector
        constexpr Int2& operator+=(Int2 rhs) noexcept { X += rhs.X; Y += rhs.Y; return *this; }
        constexpr Int2& operator-=(Int2 rhs) noexcept { X -= rhs.X; Y -= rhs.Y; return *this; }
        constexpr Int2& operator*=(Int2 rhs) noexcept { X *= rhs.X; Y *= rhs.Y; return *this; }

        //- Comparison
        friend constexpr bool operator==(Int2 lhs, Int2 rhs) noexcept { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
        friend constexpr bool operator!=(Int2 lhs, Int2 rhs) noexcept { return lhs.X != rhs.X || lhs.Y != rhs.Y; }

        //- Unary
        friend constexpr Int2 operator-(Int2 rhs) noexcept { return Int2(-rhs.X, -rhs.Y); }

        //- Binary
        //-- Vector | Scalar
        friend constexpr Int2 operator+(Int2 lhs, int32 rhs) noexcept { return Int2(lhs.X + rhs, lhs.Y + rhs); }
        friend constexpr Int2 operator-(Int2 lhs, int32 rhs) noexcept { return Int2(lhs.X - rhs, lhs.Y - rhs); }
        friend constexpr Int2 operator*(Int2 lhs, int32 rhs) noexcept { return Int2(lhs.X * rhs, lhs.Y * rhs); }
        //-- Scalar | Vector
        friend constexpr Int2 operator+(int32 lhs, Int2 rhs) noexcept { return Int2(lhs + rhs.X, lhs + rhs.Y); }
        friend constexpr Int2 operator-(int32 lhs, Int2 rhs) noexcept { return Int2(lhs - rhs.X, lhs - rhs.Y); }
        friend constexpr Int2 operator*(int32 lhs, Int2 rhs) noexcept { return Int2(lhs * rhs.X, lhs * rhs.Y); }
        //-- Vector | Vector
        friend constexpr Int2 operator+(Int2 lhs, Int2 rhs) noexcept { return Int2(lhs.X + rhs.X, lhs.Y + rhs.Y); }
        friend constexpr Int2 operator-(Int2 lhs, Int2 rhs) noexcept { return Int2(lhs.X - rhs.X, lhs.Y - rhs.Y); }
        friend constexpr Int2 operator*(Int2 lhs, Int2 rhs) noexcept { return Int2(lhs.X * rhs.X, lhs.Y * rhs.Y); }

        [[nodiscard]] static constexpr Int2 Zero() noexcept { return Int2(0); }
        [[nodiscard]] static constexpr Int2 One() noexcept { return Int2(1); }
    };

} // export namespace Copium
