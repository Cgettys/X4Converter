#include "StdInc.h"

XacSkinningChunkv3InfluenceRange::XacSkinningChunkv3InfluenceRange ()
{
    FirstInfluenceIndex = -1;
    NumInfluences = 0;
}

XacSkinningChunkv3InfluenceRange::XacSkinningChunkv3InfluenceRange ( int firstInfluenceIndex, int numInfluences )
{
    FirstInfluenceIndex = firstInfluenceIndex;
    NumInfluences = numInfluences;
}
