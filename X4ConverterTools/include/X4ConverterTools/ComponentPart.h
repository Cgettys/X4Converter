#pragma once

struct ComponentPart
{
    ComponentPartLod*                               GetLod              ( int lodIndex );

    std::string                                     Name;
    std::string                                     ParentName;
    std::vector < ComponentPartLod >                Lods;
    std::shared_ptr < XuMeshFile >                  CollisionMesh;
    aiVector3D                                      Position;
    aiVector3D                                      Center;
    aiVector3D                                      Size;
};
