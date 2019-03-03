#include "StdInc.h"

ActorSubMesh::ActorSubMesh ()
{

}

ActorSubMesh::ActorSubMesh ( ActorSubMesh&& other )
    : MaterialId        ( other.MaterialId ),
      VertexPositions   ( std::move ( other.VertexPositions ) ),
      VertexNormals     ( std::move ( other.VertexNormals ) ),
      VertexTangents    ( std::move ( other.VertexTangents ) ),
      VertexBitangents  ( std::move ( other.VertexBitangents ) ),
      VertexUVs         ( std::move ( other.VertexUVs ) ),
      VertexColors32    ( std::move ( other.VertexColors32 ) ),
      VertexColors128   ( std::move ( other.VertexColors128 ) ),
      Indices           ( std::move ( other.Indices ) )
{

}
