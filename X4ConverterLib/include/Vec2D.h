#pragma once

struct Vec2D
{
                Vec2D               ();
                Vec2D               ( const Vec2D& other );
                Vec2D               ( const aiVector2D& other );
                Vec2D               ( float x, float y );

                operator aiVector2D () const;

    float       X;
    float       Y;
};
