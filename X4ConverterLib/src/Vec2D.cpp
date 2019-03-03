#include "StdInc.h"

Vec2D::Vec2D ()
{
    X = 0.0f;
    Y = 0.0f;
}

Vec2D::Vec2D ( const Vec2D& other )
{
    X = other.X;
    Y = other.Y;
}

Vec2D::Vec2D ( const aiVector2D& other )
{
    X = other.x;
    Y = other.y;
}

Vec2D::Vec2D ( float x, float y )
{
    X = x;
    Y = y;
}

Vec2D::operator aiVector2D () const
{
    return aiVector2D ( X, Y );
}
