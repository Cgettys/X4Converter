#include "StdInc.h"

using namespace boost;

ActorNode::ActorNode ()
{
    Parent = nullptr;
}

ActorNode* ActorNode::GetNode ( const std::string& name )
{
    if ( Name == name )
        return this;

    foreach ( std::shared_ptr<ActorNode> pChildNode, Children )
    {
        ActorNode* pFoundNode = pChildNode->GetNode ( name );
        if ( pFoundNode )
            return pFoundNode;
    }
    return nullptr;
}
