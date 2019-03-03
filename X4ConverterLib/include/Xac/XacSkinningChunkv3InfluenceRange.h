#pragma once

struct XacSkinningChunkv3InfluenceRange
{
                XacSkinningChunkv3InfluenceRange    ();
                XacSkinningChunkv3InfluenceRange    ( int firstInfluenceIndex, int numInfluences );

    int         FirstInfluenceIndex;
    int         NumInfluences;
};
