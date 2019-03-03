#include "StdInc.h"

Vec3D::Vec3D ()
{
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
}

Vec3D::Vec3D ( const Vec3D& other )
{
    X = other.X;
    Y = other.Y;
    Z = other.Z;
}

Vec3D::Vec3D ( const aiVector3D& other )
{
    X = other.x;
    Y = other.y;
    Z = other.z;
}

Vec3D::Vec3D ( float x, float y, float z )
{
    X = x;
    Y = y;
    Z = z;
}

Vec3D::operator aiVector3D () const
{
    return aiVector3D ( X, Y, Z );
}

Vec3D Vec3D::operator+ ( const Vec3D& other ) const
{
    return Vec3D ( X + other.X, Y + other.Y, Z + other.Z );
}

Vec3D Vec3D::operator- ( const Vec3D& other ) const
{
    return Vec3D ( X - other.X, Y - other.Y, Z - other.Z );
}

Vec3D Vec3D::operator* ( float factor ) const
{
    return Vec3D ( X*factor, Y*factor, Z*factor );
}
