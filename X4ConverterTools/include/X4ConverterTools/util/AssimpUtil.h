#pragma once

#include <X4ConverterTools/types.h>
#include <unordered_map>
#include <assimp/scene.h>

#include <boost/numeric/conversion/cast.hpp>

class AssimpUtil {
public:
    static void MergeVertices(aiMesh *pMesh);

    struct VertexInfo {
        aiVector3D Position;
        aiVector3D Normal;
        aiVector3D UV[AI_MAX_NUMBER_OF_TEXTURECOORDS];

        bool operator==(const VertexInfo &other) const;
    };
};

namespace std {
    template<>
    class hash<aiVector3D> : public unary_function<aiVector3D, size_t> {
    public:
        typedef aiVector3D _Kty;

        size_t operator()(const _Kty &value) const {
            return *(uint32_t *) &value.x ^
                   *(uint32_t *) &value.y ^
                   *(uint32_t *) &value.z;
        }
    };

    template<>
    class hash<AssimpUtil::VertexInfo> : public unary_function<aiVector3D, size_t> {
    public:
        typedef AssimpUtil::VertexInfo _Kty;

        size_t operator()(const _Kty &value) const {
            uint32_t result = 0;
            result ^= hash<aiVector3D>()(value.Position);
            result ^= hash<aiVector3D>()(value.Normal);
            for (auto i : value.UV) {
                result ^= hash<aiVector3D>()(i);
            }
            return result;
        }
    };
}
