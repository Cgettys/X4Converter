#pragma once

struct XacMaterialDefinitionChunkv2Header
{
    Color       AmbientColor;
    Color       DiffuseColor;
    Color       SpecularColor;
    Color       EmissiveColor;
    float       Shine;
    float       ShineStrength;
    float       Opacity;
    float       IOR;
    bool        DoubleSided;
    bool        Wireframe;
    byte        _pad0;
    byte        NumLayers;
};
