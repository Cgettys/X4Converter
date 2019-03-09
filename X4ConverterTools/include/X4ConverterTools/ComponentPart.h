#pragma once
#include <X4ConverterTools/ComponentPartLod.h>
struct ComponentPart
{
    ComponentPartLod*                               GetLod              ( int lodIndex );

    std::string                                     Name;
    std::string                                     ParentName;
    std::vector < ComponentPartLod >                Lods;
    XuMeshFile*                                     CollisionMesh;
    aiVector3D                                      Position;
    aiVector3D                                      Center;
    aiVector3D                                      Size;
};
