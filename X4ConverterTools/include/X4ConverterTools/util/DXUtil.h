#pragma once

#include <X4ConverterTools/types.h>

namespace util {
    class DXUtil {
    public:
        static int GetVertexElementTypeSize(D3DDECLTYPE type);

        static Vec3D ConvertVertexAttributeToVec3D(byte *pAttribute, D3DDECLTYPE type);

        static aiColor4D ConvertVertexAttributeToColorF(byte *pAttribute, D3DDECLTYPE type);

        static int WriteVec3DToVertexAttribute(aiVector3D vector, D3DDECLTYPE type, byte *pAttribute);

        static int WriteColorFToVertexAttribute(const aiColor4D &color, D3DDECLTYPE type, byte *pAttribute);
    };
}