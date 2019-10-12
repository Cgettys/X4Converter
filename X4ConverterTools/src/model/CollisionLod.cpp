#include "X4ConverterTools/model/CollisionLod.h"
#include <iostream>
#include <boost/format.hpp>
#include <utility>

namespace model {
// Not really a LOD but it makes more sense if we pretend
CollisionLod::CollisionLod(ConversionContext::Ptr ctx) : Lod(std::move(ctx)) {}

CollisionLod::CollisionLod(std::string partName, const ConversionContext::Ptr &ctx) : Lod(ctx) {
  index = COLLISION_INDEX;
  std::string name = str(boost::format("%1%-collision") % partName);
  setName(name);
  auto pStream = ctx->GetSourceFile(name + ".xmf");
  xmfFile = xmf::XmfFile::ReadFromIOStream(pStream, ctx);
}

void CollisionLod::ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) {
  std::string rawName = node->mName.C_Str();
  setName(rawName);
  // Parse out the index
  size_t pos = rawName.rfind("-collision");
  if (pos == std::string::npos) {
    throw std::runtime_error("this is not a collision mesh");
  }
  index = COLLISION_INDEX;
  xmfFile = xmf::XmfFile::GenerateMeshFile(ctx, node, true);
  CalculateSizeAndCenter(node);
}

void CollisionLod::CalculateSizeAndCenter(aiNode *pCollisionNode) {
  if (pCollisionNode->mNumMeshes == 0)
    return;

  aiVector3D lowerBound;
  aiVector3D upperBound;
  aiMesh *pCollisionMesh = ctx->GetMesh(pCollisionNode->mMeshes[0]);
  for (size_t i = 0; i < pCollisionMesh->mNumVertices; ++i) {
    aiVector3D &position = pCollisionMesh->mVertices[i];
    if (i == 0) {
      lowerBound = position;
      upperBound = position;
    } else {
      lowerBound.x = std::min(lowerBound.x, position.x);
      lowerBound.y = std::min(lowerBound.y, position.y);
      lowerBound.z = std::min(lowerBound.z, position.z);

      upperBound.x = std::max(upperBound.x, position.x);
      upperBound.y = std::max(upperBound.y, position.y);
      upperBound.z = std::max(upperBound.z, position.z);
    }
  }

  // TODO flip axes if necessary
  maxDim = aiVector3D((upperBound.x - lowerBound.x) / 2.0f, (upperBound.y - lowerBound.y) / 2.0f,
                      (upperBound.z - lowerBound.z) / 2.0f);
  center = aiVector3D(lowerBound.x + maxDim.x, lowerBound.y + maxDim.y, lowerBound.z + maxDim.z);
}

void CollisionLod::ConvertToGameFormat(pugi::xml_node out) {
  // TODO handle no collision mesh
  if (maxDim != aiVector3D(0, 0, 0) || center != aiVector3D(0, 0, 0)) {
    auto sizeNode = AddChild(out, "size");

    auto maxNode = AddChild(sizeNode, "max");
    WriteAttrXYZ(maxNode, maxDim);
    auto centerNode = AddChild(sizeNode, "center");
    WriteAttrXYZ(centerNode, center);
  } else {
    out.remove_child("size");
    out.remove_child("size_raw");
  }
  // TODO size raw???
  xmfFile->WriteToFile(getName() + ".out.xmf");
}
}