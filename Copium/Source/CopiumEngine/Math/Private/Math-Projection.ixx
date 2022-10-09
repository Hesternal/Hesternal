export module CopiumEngine.Math:Projection;

import :Float4x4;
import CopiumEngine.Core.CoreTypes;

import <cmath>;


export namespace Copium::Math
{

    // NOTE(v.matushkin): At least I hope that is LH and maps Z to the right range
    /// LeftHanded, maps ZNear/ZFar to [0..1] range
    [[nodiscard]] constexpr Float4x4 OrthoOffCenter(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far) noexcept
    {
        float32 rcpX = 1.0f / (right - left);
        float32 rcpY = 1.0f / (top - bottom);
        float32 rcpZ = 1.0f / (far - near);

        return Float4x4(2.0f * rcpX,        0.0f, 0.0f, (right + left) * -rcpX,
                               0.0f, 2.0f * rcpY, 0.0f, (top + bottom) * -rcpY,
                               0.0f,        0.0f, rcpZ,           near * -rcpZ,
                               0.0f,        0.0f, 0.0f,                   1.0f);
    }

    // NOTE(v.matushkin): Also not sure about the way I build Perspective matrix, seems like it's also wrong?
    /// verticalFov in radians
    [[nodiscard]] Float4x4 Perspective(float32 verticalFov, float32 aspect, float32 near, float32 far) noexcept
    {
        float32 cotangent = 1.0f / std::tanf(verticalFov * 0.5f);
        float32 rcp = 1.0f / (near - far);

        float32 m00 = cotangent / aspect;
        float32 m22 = (far + near) * rcp;
        float32 m23 = 2.0f * near * far * rcp;

        return Float4x4( m00,      0.0f,  0.0f, 0.0f,
                        0.0f, cotangent,  0.0f, 0.0f,
                        0.0f,      0.0f,   m22,  m23,
                        0.0f,      0.0f, -1.0f, 0.0f);
    }

} // export namespace Copium::Math
