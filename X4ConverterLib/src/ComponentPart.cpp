#include "StdInc.h"

ComponentPartLod* ComponentPart::GetLod ( int lodIndex )
{
    foreach ( ComponentPartLod& lod, Lods )
    {
        if ( lod.LodIndex == lodIndex )
            return &lod;
    }
    return nullptr;
}
