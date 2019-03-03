#pragma once

class ActorNodeDeformation
{
public:
    ActorNode*              Node;
    std::vector < int >     VertexIndices;
    std::vector < Vec3D >   PositionOffsets;
    std::vector < Vec3D >   NormalOffsets;
    std::vector < Vec3D >   TangentOffsets;
};
