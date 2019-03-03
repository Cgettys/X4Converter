#include "StdInc.h"

Quaternion::Quaternion ()
{
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
    W = 1.0f;
}

Quaternion::Quaternion ( const Quaternion& other )
{
    X = other.X;
    Y = other.Y;
    Z = other.Z;
    W = other.W;
}

Quaternion::Quaternion ( const aiQuaternion& other )
{
    X = other.x;
    Y = other.y;
    Z = other.z;
    W = other.w;
}

Quaternion::Quaternion ( float x, float y, float z, float w )
{
    X = x;
    Y = y;
    Z = z;
    W = w;
}

Quaternion::operator aiQuaternion () const
{
    return aiQuaternion ( W, X, Y, Z );
}
