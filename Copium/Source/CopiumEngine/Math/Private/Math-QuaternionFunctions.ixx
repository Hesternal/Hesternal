export module CopiumEngine.Math:QuaternionFunctions;

import :Quaternion;

import Copium.Core;


export namespace Copium::Math
{

    [[nodiscard]] constexpr float32 Dot(const Quaternion& lhs, const Quaternion& rhs) noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W; }

} // export namespace Copium::Math
