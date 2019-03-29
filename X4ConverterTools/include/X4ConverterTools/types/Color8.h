#pragma once

#include <assimp/scene.h>
#include <X4ConverterTools/types.h>

struct Color8 {
    Color8();

    Color8(const Color8 &other);

    explicit Color8(const aiColor4D &other);
//            Color8              ( byte r, byte g, byte b, byte a );

    explicit operator aiColor4D() const;

    byte R;
    byte G;
    byte B;
    byte A;
};
