#pragma once

struct XacMeshChunkv1VertexElement
{
    int         Usage;
    int         ElementSize;
    bool        KeepOriginals;
    bool        IsScaleFactor;
    byte        _pad0[2];
};
