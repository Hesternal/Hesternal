export module Hesternal.Math:Projection;

import :Float4x4;
import :Functions;

import Hesternal.Core;


export namespace Hesternal::Math
{

    // TODO(v.matushkin): <ReverseZ> HOW TO DO IT HERE?
    /// LeftHanded, maps ZNear/ZFar to [0..1] range
    [[nodiscard]] constexpr Float4x4 OrthoOffCenter(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far) noexcept
    {
        const float32 rcpX = 1.0f / (right - left);
        const float32 rcpY = 1.0f / (top - bottom);
        const float32 rcpZ = 1.0f / (far - near);

        return Float4x4(2.0f * rcpX,        0.0f, 0.0f, (right + left) * -rcpX,
                               0.0f, 2.0f * rcpY, 0.0f, (top + bottom) * -rcpY,
                               0.0f,        0.0f, rcpZ,           near * -rcpZ,
                               0.0f,        0.0f, 0.0f,                   1.0f);
    }

    // NOTE(v.matushkin): <ReverseZ>
    /// verticalFov in radians
    [[nodiscard]] inline Float4x4 Perspective(float32 verticalFov, float32 aspect, float32 near, float32 far) noexcept
    {
        const float32 cotangent = 1.0f / Tan(verticalFov * 0.5f);
        const float32 rcpZ = 1.0f / (far - near);

        const float32 m00 = cotangent / aspect;
        const float32 m22 = near * rcpZ;
        const float32 m23 = (far * near) * rcpZ;

        return Float4x4( m00,      0.0f,  0.0f, 0.0f,
                        0.0f, cotangent,  0.0f, 0.0f,
                        0.0f,      0.0f,  -m22,  m23,
                        0.0f,      0.0f,  1.0f, 0.0f);

        // Not reversed z
        //const float32 m22 = far * rcpZ;
        //return Float4x4( m00,      0.0f,  0.0f, 0.0f,
        //                0.0f, cotangent,  0.0f, 0.0f,
        //                0.0f,      0.0f,   m22, -m23,
        //                0.0f,      0.0f,  1.0f, 0.0f);
    }

} // export namespace Hesternal::Math
