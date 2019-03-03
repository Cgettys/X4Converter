#pragma once

struct XacMaterialDefinitionChunkv2Layer
{
    float       Amount;
    float       UOffset;
    float       VOffset;
    float       UTiling;
    float       VTiling;
    float       Rotation;       // radians
    short       MaterialId;
    byte        MapType;
    byte        _pad0;
};
