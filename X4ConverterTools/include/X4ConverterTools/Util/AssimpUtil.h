#pragma once

class AssimpUtil
{
public:
    static void             MergeVertices           ( aiMesh* pMesh );

    struct VertexInfo
    {
        aiVector3D      Position;
        aiVector3D      Normal;
        aiVector3D      UV[AI_MAX_NUMBER_OF_TEXTURECOORDS];

        bool            operator==      ( const VertexInfo& other ) const;
    };
};

namespace std
{
    template <>
    class hash<aiVector3D> : public unary_function<aiVector3D, size_t>
    {
    public:
        typedef aiVector3D _Kty;

        size_t operator()(const _Kty& value) const
        {
            return *(uint *)&value.x ^
                   *(uint *)&value.y ^
                   *(uint *)&value.z;
        }
    };

    template<>
    class hash<AssimpUtil::VertexInfo> : public unary_function<aiVector3D, size_t>
    {
    public:
        typedef AssimpUtil::VertexInfo _Kty;

        size_t operator()(const _Kty& value) const
        {
            uint result = 0;
            result ^= hash < aiVector3D > () ( value.Position );
            result ^= hash < aiVector3D > () ( value.Normal );
            for ( int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i )
            {
                result ^= hash < aiVector3D > () ( value.UV[i] );
            }
            return result;
        }
    };
};
