#pragma once

struct Vec3D
{
                Vec3D                   ();
                Vec3D                   ( const Vec3D& other );
                Vec3D                   ( const aiVector3D& other );
                Vec3D                   ( float x, float y, float z );
                operator aiVector3D     () const;

    Vec3D       operator+               ( const Vec3D& other ) const;
    Vec3D       operator-               ( const Vec3D& other ) const;
    Vec3D       operator*               ( float factor ) const;

    float       X;
    float       Y;
    float       Z;
};
