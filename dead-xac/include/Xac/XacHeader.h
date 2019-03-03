#pragma once

#pragma pack(push)
#pragma pack(1)

struct XacHeader
{
    byte        Magic[4];
    byte        MajorVersion;
    byte        MinorVersion;
    bool        BigEndian;
    byte        MultiplyOrder;
};

#pragma pack(pop)
