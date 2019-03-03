#include "StdInc.h"

Color::Color ()
{
    R = 0.0f;
    G = 0.0f;
    B = 0.0f;
    A = 0.0f;
}

Color::Color ( const Color& other )
{
    R = other.R;
    G = other.G;
    B = other.B;
    A = other.A;
}

Color::Color ( const aiColor4D& other )
{
    R = other.r;
    G = other.g;
    B = other.b;
    A = other.a;
}

Color::Color ( float r, float g, float b, float a )
{
    R = r;
    G = g;
    B = b;
    A = a;
}

Color::operator aiColor4D () const
{
    return aiColor4D ( R, G, B, A );
}
