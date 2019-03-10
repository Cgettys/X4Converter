#pragma once

#pragma pack(push)
#pragma pack(1)

struct XmfDataBufferDesc
{
    dword               Type;
    dword               UsageIndex;
    dword               DataOffset;
    dword               Compressed;
    byte                _pad0[4] = {0,0,0,0};
    dword               Format;
    dword               CompressedDataSize;
    dword               NumItemsPerSection;
    dword               ItemSize;
    dword               NumSections;
    byte                _pad1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    dword               NumVertexElements;
    XmfVertexElement    VertexElements[16];
};

#pragma pack(pop)