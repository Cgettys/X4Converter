#pragma once

class ActorMaterial
{
public:
                        ActorMaterial           ();

    std::string         Name;
    Color               AmbientColor;
    Color               DiffuseColor;
    Color               SpecularColor;
    Color               EmissiveColor;
    float               Shine;
    float               ShineStrength;
    float               Opacity;
    float               IOR;
    bool                DoubleSided;
    bool                Wireframe;

    std::vector < ActorMaterialLayer >  Layers;
};
