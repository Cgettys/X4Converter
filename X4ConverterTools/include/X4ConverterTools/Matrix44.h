#pragma once

#include "Vec4D.h"

struct Matrix44 {
    explicit operator aiMatrix4x4();

    Vec4D Axes[3];
    Vec4D Position;
};
