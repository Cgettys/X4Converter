#pragma once

struct Color
{
                Color                   ();
                Color                   ( const Color& other );
                Color                   ( const aiColor4D& other );
                Color                   ( float r, float g, float b, float a );
                operator aiColor4D      () const;

    float       R;
    float       G;
    float       B;
    float       A;
};
