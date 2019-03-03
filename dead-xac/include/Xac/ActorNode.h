#pragma once

class ActorNode
{
public:
                                                    ActorNode       ();
    ActorNode*                                      GetNode         ( const std::string& name );

    std::string                                     Name;
    Vec3D                                           Position;
    Quaternion                                      Rotation;
    Vec3D                                           Scale;
    Quaternion                                      ScaleRotation;
    Matrix44                                        Transform;

    ActorNode*                                      Parent;
    std::vector < std::shared_ptr < ActorNode > >   Children;

    std::shared_ptr < ActorMesh >                   VisualMesh;
    std::shared_ptr < ActorMesh >                   CollisionMesh;
};
