#pragma once

class ActorSubMesh
{
public:
                                                    ActorSubMesh            ();
                                                    ActorSubMesh            ( ActorSubMesh&& other );

    int                                             MaterialId;

    std::vector < Vec3D >                           VertexPositions;
    std::vector < Vec3D >                           VertexNormals;
    std::vector < Vec4D >                           VertexTangents;
    std::vector < Vec4D >                           VertexBitangents;
    std::vector < std::vector < Vec2D > >           VertexUVs;
    std::vector < std::vector < Color8 > >          VertexColors32;
    std::vector < std::vector < Color > >           VertexColors128;
    std::vector < std::vector < BoneInfluence > >   VertexInfluences;
    
    std::vector < int >                             Indices;
};
