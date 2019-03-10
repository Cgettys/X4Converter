#pragma once
#include <assimp/scene.h>
struct Color
{
                Color                   ();
                Color                   ( const Color& other );

       explicit Color                   ( const aiColor4D& other );
                Color                   ( float r, float g, float b, float a );

    explicit operator aiColor4D      () const;

    float       R;
    float       G;
    float       B;
    float       A;
};
