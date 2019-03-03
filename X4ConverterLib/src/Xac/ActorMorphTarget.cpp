#include "StdInc.h"

using namespace boost;

ActorMorphTarget::ActorMorphTarget ()
{
    RangeMin = 0.0f;
    RangeMax = 1.0f;
    PhonemeSetBitmask = 0;
}

ActorNodeDeformation* ActorMorphTarget::GetDeformation ( ActorNode* pNode )
{
    foreach ( ActorNodeDeformation& deformation, Deformations )
    {
        if ( deformation.Node == pNode )
            return &deformation;
    }
    return nullptr;
}
