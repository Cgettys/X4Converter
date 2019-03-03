#pragma once

struct XacSkinningChunkv3Header
{
    int         NodeId;
    int         NumLocalBones;
    int         NumInfluences;
    bool        IsForCollisionMesh;
    byte        _pad0[3];
};
