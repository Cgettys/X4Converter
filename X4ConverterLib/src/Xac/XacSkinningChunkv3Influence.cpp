#include "StdInc.h"

XacSkinningChunkv3Influence::XacSkinningChunkv3Influence ()
{
    NodeId = -1;
    Weight = 0.0f;
}

XacSkinningChunkv3Influence::XacSkinningChunkv3Influence ( short nodeId, float weight )
{
    NodeId = nodeId;
    Weight = weight;
}
