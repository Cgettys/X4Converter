#pragma once

struct Quaternion
{
                Quaternion                  ();
                Quaternion                  ( const Quaternion& other);
                Quaternion                  ( const aiQuaternion& other );
                Quaternion                  ( float x, float y, float z, float w );
                operator aiQuaternion       () const;

    float       X;
    float       Y;
    float       Z;
    float       W;
};
