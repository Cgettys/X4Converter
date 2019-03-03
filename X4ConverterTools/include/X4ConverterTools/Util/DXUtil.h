#pragma once

class DXUtil
{
public:
    static int          GetVertexElementTypeSize            ( D3DDECLTYPE type );

    static Vec3D        ConvertVertexAttributeToVec3D       ( byte* pAttribute, D3DDECLTYPE type );
    static Color        ConvertVertexAttributeToColorF      ( byte* pAttribute, D3DDECLTYPE type );

    static int          WriteVec3DToVertexAttribute         ( const Vec3D& vector, D3DDECLTYPE type, byte* pAttribute );
    static int          WriteColorFToVertexAttribute        ( const Color& color, D3DDECLTYPE type, byte* pAttribute );
};
