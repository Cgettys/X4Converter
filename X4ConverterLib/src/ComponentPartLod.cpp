#include "StdInc.h"

ComponentPartLod::ComponentPartLod ( int lodIndex, std::shared_ptr<XuMeshFile> pMesh )
    : LodIndex ( lodIndex ), Mesh ( pMesh )
{

}
