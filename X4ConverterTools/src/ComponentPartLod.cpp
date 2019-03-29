#include <X4ConverterTools/ComponentPartLod.h>
ComponentPartLod::ComponentPartLod(int lodIndex, std::shared_ptr<xmf::XuMeshFile> pMesh)
        : LodIndex(lodIndex), Mesh(std::move(pMesh)) {

}
