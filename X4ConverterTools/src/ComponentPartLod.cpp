#include <X4ConverterTools/StdInc.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>

ComponentPartLod::ComponentPartLod(int lodIndex, std::shared_ptr<xmf::XuMeshFile> pMesh)
        : LodIndex(lodIndex), Mesh(std::move(pMesh)) {

}
