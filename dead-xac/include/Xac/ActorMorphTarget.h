#pragma once

class ActorMorphTarget
{
public:
                                            ActorMorphTarget                ();

    ActorNodeDeformation*                   GetDeformation                  ( ActorNode* pNode );

    std::string                             Name;
    float                                   RangeMin;
    float                                   RangeMax;
    int                                     PhonemeSetBitmask;
    std::vector < ActorNodeDeformation >    Deformations;
};
