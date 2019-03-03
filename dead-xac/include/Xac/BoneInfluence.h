#pragma once

class ActorNode;

struct BoneInfluence
{
                BoneInfluence       ( ActorNode* pNode, float weight );

    ActorNode*  Node;
    float       Weight;
};
