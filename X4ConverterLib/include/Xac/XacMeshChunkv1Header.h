#pragma once

struct XacMeshChunkv1Header
{
    int         NodeId;
    int         NumInfluenceRanges;
    int         NumVertices;
    int         NumIndices;
    int         NumSubMeshes;
    int         NumVertexElements;
    bool        IsCollisionMesh;
    byte        _pad0[3];
};
