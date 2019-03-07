#pragma once
#include "Vec4D.h"
struct Matrix44
{
                operator aiMatrix4x4        ();

    Vec4D       Axes[3];
    Vec4D       Position;
};
