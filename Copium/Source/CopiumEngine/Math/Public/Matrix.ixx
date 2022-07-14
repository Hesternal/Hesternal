export module CopiumEngine.Math.Matrix;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Math.Vector;


export namespace Copium
{

    struct Matrix4x4f final
    {
        Vector4f X, Y, Z, W;


        Matrix4x4f() = default;
        Matrix4x4f(const Matrix4x4f& m) = default;

        Matrix4x4f(const Vector4f& x, const Vector4f& y, const Vector4f& z, const Vector4f& w);
        Matrix4x4f(float32 _00, float32 _01, float32 _02, float32 _03,
                   float32 _10, float32 _11, float32 _12, float32 _13,
                   float32 _20, float32 _21, float32 _22, float32 _23,
                   float32 _30, float32 _31, float32 _32, float32 _33);

        // Matrix4x4f& operator+=(const Matrix4x4f& m);
        // Matrix4x4f& operator-=(const Matrix4x4f& m);
        // Matrix4x4f& operator*=(const Matrix4x4f& m);

        // Matrix4x4f operator+(const Matrix4x4f& m) const;
        // Matrix4x4f operator-(const Matrix4x4f& m) const;
        // Matrix4x4f operator*(const Matrix4x4f& m) const;

        static Matrix4x4f Zero();
        static Matrix4x4f Identity();
    };

} // export namespace Copium


namespace Copium
{

    Matrix4x4f::Matrix4x4f(const Vector4f& x, const Vector4f& y, const Vector4f& z, const Vector4f& w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
    {}

    Matrix4x4f::Matrix4x4f(float32 _00, float32 _01, float32 _02, float32 _03,
                           float32 _10, float32 _11, float32 _12, float32 _13,
                           float32 _20, float32 _21, float32 _22, float32 _23,
                           float32 _30, float32 _31, float32 _32, float32 _33)
        : X(_00, _01, _02, _03)
        , Y(_10, _11, _12, _13)
        , Z(_20, _21, _22, _23)
        , W(_30, _31, _32, _33)
    {}


    Matrix4x4f Matrix4x4f::Zero()
    {
        return Matrix4x4f(0.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 0.0f);
    }

    Matrix4x4f Matrix4x4f::Identity()
    {
        return Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);
    }

} // namespace Copium
