#pragma once
#include <X4ConverterTools/Xmf/XuMeshFile.h>
struct ComponentPartLod
{
                                    ComponentPartLod    ( int lodIndex, XuMeshFile* pMesh );

    int                             LodIndex;
    XuMeshFile *Mesh;
};
