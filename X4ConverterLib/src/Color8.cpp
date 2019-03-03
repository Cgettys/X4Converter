#include "StdInc.h"

Color8::Color8 ()
{
    R = 0;
    G = 0;
    B = 0;
    A = 0;
}

Color8::Color8 ( const Color8& other )
{
    R = other.R;
    G = other.G;
    B = other.B;
    A = other.A;
}

Color8::Color8 ( const aiColor4D& other )
{
    R = (byte)(other.r * 255.0f);
    G = (byte)(other.g * 255.0f);
    B = (byte)(other.b * 255.0f);
    A = (byte)(other.a * 255.0f);
}

Color8::operator aiColor4D () const
{
    return aiColor4D ( (float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f, (float)A / 255.0f );
}
