#pragma once

#include <X4ConverterTools/types/Vec3D.h>

class Vec4D {
public:
    Vec4D();

    explicit Vec4D(const Vec3D &other);

    Vec4D(const Vec4D &other);

    explicit Vec4D(const aiVector3D &other);

    Vec4D(float x, float y, float z, float w);

    explicit operator aiVector3D();

    float X;
    float Y;
    float Z;
    float W;
};
