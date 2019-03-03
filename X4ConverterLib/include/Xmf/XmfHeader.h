#pragma once

#pragma pack(push)
#pragma pack(1)

struct XmfHeader
{
    byte                Magic[4];
    byte                Version;
    bool                BigEndian;
    byte                DataBufferDescOffset;
    byte                _pad0;
    byte                NumDataBuffers;
    byte                DataBufferDescSize;
    byte                NumMaterials;
    byte                MaterialSize;
    byte                _pad1[10];
    dword               PrimitiveType;
};

#pragma pack(pop)
