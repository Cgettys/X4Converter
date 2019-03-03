#pragma once

struct XacNodeHierarchyChunkv1NodeHeader
{
public:
    Quaternion      Rotation;
    Quaternion      ScaleRotation;
    Vec3D           Position;
    Vec3D           Scale;
    float           _pad0[3];
    int             UnknownIndex1;
    int             UnknownIndex2;
    int             ParentNodeId;
    int             NumChildren;
    int             IncludeInBoundsCalc;
    Matrix44        Transform;
    float           ImportanceFactor;
};
