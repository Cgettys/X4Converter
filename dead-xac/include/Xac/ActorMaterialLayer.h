#pragma once

class ActorMaterialLayer
{
public:
                    ActorMaterialLayer      ();

    std::string     Texture;
    float           Amount;
    float           UOffset;
    float           VOffset;
    float           UTiling;
    float           VTiling;
    float           Rotation;
    byte            MapType;    
};
