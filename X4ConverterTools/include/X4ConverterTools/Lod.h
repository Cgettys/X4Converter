#pragma once

#include <X4ConverterTools/xmf/XuMeshFile.h>

class Lod {
public:
    Lod(int lodIndex, std::shared_ptr<xmf::XuMeshFile> pMesh);

    int Index;
    std::shared_ptr<xmf::XuMeshFile> Mesh;
};
