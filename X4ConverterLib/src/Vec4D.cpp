#include "StdInc.h"

Vec4D::Vec4D ()
{
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
    W = 0.0f;
}

Vec4D::Vec4D ( const Vec3D& other )
{
    X = other.X;
    Y = other.Y;
    Z = other.Z;
    W = 0.0f;
}

Vec4D::Vec4D ( const Vec4D& other )
{
    X = other.X;
    Y = other.Y;
    Z = other.Z;
    W = other.W;
}

Vec4D::Vec4D ( const aiVector3D& other )
{
    X = other.x;
    Y = other.y;
    Z = other.z;
    W = 0.0f;
}

Vec4D::Vec4D ( float x, float y, float z, float w )
{
    X = x;
    Y = y;
    Z = z;
    W = w;
}

Vec4D::operator aiVector3D ()
{
    return aiVector3D ( X, Y, Z );
}
