#include <X4ConverterTools/Lod.h>

Lod::Lod(int lodIndex, std::shared_ptr<xmf::XuMeshFile> pMesh) : Index(lodIndex),
                                                                                           Mesh(std::move(pMesh)) {

}

