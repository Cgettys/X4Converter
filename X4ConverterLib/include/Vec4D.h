#pragma once

struct Vec4D
{
                Vec4D               ();
                Vec4D               ( const Vec3D& other );
                Vec4D               ( const Vec4D& other);
                Vec4D               ( const aiVector3D& other );
                Vec4D               ( float x, float y, float z, float w );

                operator aiVector3D ();

    float       X;
    float       Y;
    float       Z;
    float       W;
};
