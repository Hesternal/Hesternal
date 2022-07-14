export module CopiumEngine.Math.Vector;

import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    struct Vector4f final
    {
        float32 X, Y, Z, W;


        Vector4f() = default;
        Vector4f(const Vector4f& v) = default;

        Vector4f(float32 x, float32 y, float32 z, float32 w);
        explicit Vector4f(float32 scalar);

        Vector4f operator-() const;

        Vector4f& operator+=(float32 scalar);
        Vector4f& operator-=(float32 scalar);
        Vector4f& operator*=(float32 scalar);
        Vector4f& operator+=(const Vector4f& v);
        Vector4f& operator-=(const Vector4f& v);
        Vector4f& operator*=(const Vector4f& v);

        Vector4f operator+(float32 scalar) const;
        Vector4f operator-(float32 scalar) const;
        Vector4f operator*(float32 scalar) const;
        Vector4f operator+(const Vector4f& v) const;
        Vector4f operator-(const Vector4f& v) const;
        Vector4f operator*(const Vector4f& v) const;
    };

} // export namespace Copium


namespace Copium
{

    Vector4f::Vector4f(float32 x, float32 y, float32 z, float32 w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
    {}

    Vector4f::Vector4f(float32 scalar)
        : X(scalar)
        , Y(scalar)
        , Z(scalar)
        , W(scalar)
    {}


    Vector4f Vector4f::operator-() const
    {
        return Vector4f(-X, -Y, -Z, -W);
    }

#define COMPOUND_VECTOR_SCALAR(op)                  \
    Vector4f& Vector4f::operator op(float32 scalar) \
    {                                               \
        X op scalar;                                \
        Y op scalar;                                \
        Z op scalar;                                \
        W op scalar;                                \
        return *this;                               \
    }

#define COMPOUND_VECTOR_VECTOR(op)                     \
    Vector4f& Vector4f::operator op(const Vector4f& v) \
    {                                                  \
        X op v.X;                                      \
        Y op v.Y;                                      \
        Z op v.Z;                                      \
        W op v.W;                                      \
        return *this;                                  \
    }

#define BINARY_VECTOR_SCALAR(op)                                             \
    Vector4f Vector4f::operator op(float32 scalar) const                     \
    {                                                                        \
        return Vector4f(X op scalar, Y op scalar, Z op scalar, W op scalar); \
    }

#define BINARY_VECTOR_VECTOR(op)                                 \
    Vector4f Vector4f::operator op(const Vector4f& v) const      \
    {                                                            \
        return Vector4f(X op v.X, Y op v.Y, Z op v.Z, W op v.W); \
    }

    COMPOUND_VECTOR_SCALAR(+=)
    COMPOUND_VECTOR_SCALAR(-=)
    COMPOUND_VECTOR_SCALAR(*=)

    COMPOUND_VECTOR_VECTOR(+=)
    COMPOUND_VECTOR_VECTOR(-=)
    COMPOUND_VECTOR_VECTOR(*=)

    BINARY_VECTOR_SCALAR(+)
    BINARY_VECTOR_SCALAR(-)
    BINARY_VECTOR_SCALAR(*)

    BINARY_VECTOR_VECTOR(+)
    BINARY_VECTOR_VECTOR(-)
    BINARY_VECTOR_VECTOR(*)

#undef COMPOUND_VECTOR_SCALAR
#undef BINARY_VECTOR_SCALAR
#undef BINARY_VECTOR_VECTOR

} // namespace Copium
