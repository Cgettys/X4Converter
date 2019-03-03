#pragma once

struct XacSkinningChunkv3Influence
{
                XacSkinningChunkv3Influence     ();
                XacSkinningChunkv3Influence     ( short nodeId, float weight );

    float       Weight;
    short       NodeId;
    byte        _pad0[2];
};
