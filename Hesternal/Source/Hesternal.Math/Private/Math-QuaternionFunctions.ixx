export module Hesternal.Math:QuaternionFunctions;

import :Quaternion;

import Hesternal.Core;


export namespace Hesternal::Math
{

    [[nodiscard]] constexpr float32 Dot(const Quaternion& lhs, const Quaternion& rhs) noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W; }

} // export namespace Hesternal::Math
