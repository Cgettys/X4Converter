#pragma once

struct XacMetaDataChunkv2Header
{
public:
    uint        RepositionMask;
    int         RepositioningNode;
    byte        ExporterMajorVersion;
    byte        ExporterMinorVersion;
    byte        _pad[2];
    float       RetargetRootOffset;
};
