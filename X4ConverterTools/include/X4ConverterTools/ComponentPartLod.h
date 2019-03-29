#pragma once

#include <X4ConverterTools/xmf/XuMeshFile.h>

struct ComponentPartLod {
    ComponentPartLod(int lodIndex, std::shared_ptr<xmf::XuMeshFile> pMesh);

    int LodIndex;
    std::shared_ptr<xmf::XuMeshFile> Mesh;
};
