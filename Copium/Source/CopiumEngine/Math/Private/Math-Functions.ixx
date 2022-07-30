export module CopiumEngine.Math:Functions;

import :Constants;
import CopiumEngine.Core.CoreTypes;

import <cmath>;


export namespace Copium::Math
{

    void SinCos(float32 x, float32& sin, float32& cos) { sin = std::sin(x); cos = std::cos(x); }

    [[nodiscard]] constexpr float32 Radians(float32 degrees) { return degrees * k_DegreeToRadians; }
    [[nodiscard]] constexpr float32 Degrees(float32 radians) { return radians * k_RadiansToDegree; }

} // export namespace Copium::Math
