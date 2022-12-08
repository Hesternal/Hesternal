export module Copium.Math:Constants;

import Copium.Core;

import <numbers>;


export namespace Copium::Math::Constants
{

    inline constexpr float32 PI = std::numbers::pi_v<float32>;

    inline constexpr float32 Deg2Rad = PI / 180.0f;
    // NOTE(v.matushkin): IntelliSense lies about division by zero, because I import <numbers> as a header unit
    inline constexpr float32 Rad2Deg = 180.0f / PI;

} // export namespace Copium::Math::Constants
