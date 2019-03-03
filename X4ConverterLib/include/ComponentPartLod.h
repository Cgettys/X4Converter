#pragma once

struct ComponentPartLod
{
                                    ComponentPartLod    ( int lodIndex, std::shared_ptr<XuMeshFile> pMesh );

    int                             LodIndex;
    std::shared_ptr<XuMeshFile>     Mesh;
};
