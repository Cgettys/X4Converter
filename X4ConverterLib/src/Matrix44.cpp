#include "StdInc.h"

Matrix44::operator aiMatrix4x4 ()
{
    aiMatrix4x4 result;

    result.a1 = Axes[0].X;
    result.a2 = Axes[1].X;
    result.a3 = Axes[2].X;
    result.a4 = Position.X;

    result.b1 = Axes[0].Y;
    result.b2 = Axes[1].Y;
    result.b3 = Axes[2].Y;
    result.b4 = Position.Y;

    result.c1 = Axes[0].Z;
    result.c2 = Axes[1].Z;
    result.c3 = Axes[2].Z;
    result.c4 = Position.Z;

    result.d1 = Axes[0].W;
    result.d2 = Axes[1].W;
    result.d3 = Axes[2].W;
    result.d4 = Position.W;

    return result;
}
