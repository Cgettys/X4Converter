#include "X4ConverterTools/model/CollisionLod.h"
#include <iostream>
#include <boost/format.hpp>
#include <utility>

namespace model {
namespace xml = util::xml;
// Not really a LOD but it makes more sense if we pretend
CollisionLod::CollisionLod(ConversionContext::Ptr ctx) : Lod(std::move(ctx)) {}

CollisionLod::CollisionLod(std::string partName, const ConversionContext::Ptr &ctx) : Lod(ctx) {
  index = COLLISION_INDEX;
  std::string name = str(boost::format("%1%-collision") % partName);
  setName(name);
  if (ctx->ShouldConvertGeometry()) {
    xmfFile = xmf::XmfFile::ReadFromFile(name + ".xmf", ctx);
  }
}

void CollisionLod::ConvertFromAiNode(aiNode *node) {
  std::string rawName = node->mName.C_Str();
  setName(rawName);
  // Parse out the index
  size_t pos = rawName.rfind("-collision");
  if (pos == std::string::npos) {
    throw std::runtime_error("this is not a collision mesh");
  }
  index = COLLISION_INDEX;
  if (ctx->ShouldConvertGeometry()) {
    xmfFile = xmf::XmfFile::GenerateMeshFile(ctx, node, true);
    CalculateSizeAndCenter(node);
  }
}

void CollisionLod::CalculateSizeAndCenter(aiNode *pCollisionNode) {
  auto numMeshes = pCollisionNode->mNumMeshes;
  if (numMeshes != 1) {
    std::stringstream ss;
    ss << "Warning, collision " << getName() << " has " << numMeshes << " meshes. Should have one." << std::endl;
    throw std::runtime_error(ss.str());
  }

  aiMesh *pCollisionMesh = ctx->GetMesh(pCollisionNode->mMeshes[0]);
  auto numVertices = pCollisionMesh->mNumVertices;
  if (numVertices == 0) {
    std::stringstream ss;
    ss << "Warning, collision " << getName() << " has an empty mesh." << std::endl;
    throw std::runtime_error(ss.str());
  }

  aiVector3D lowerBound = pCollisionMesh->mVertices[0];
  aiVector3D upperBound = pCollisionMesh->mVertices[0];
  for (size_t i = 1; i < numVertices; ++i) {
    aiVector3D &position = pCollisionMesh->mVertices[i];
    lowerBound.x = std::min(lowerBound.x, position.x);
    lowerBound.y = std::min(lowerBound.y, position.y);
    lowerBound.z = std::min(lowerBound.z, position.z);

    upperBound.x = std::max(upperBound.x, position.x);
    upperBound.y = std::max(upperBound.y, position.y);
    upperBound.z = std::max(upperBound.z, position.z);
  }

  // TODO flip axes if necessary
  maxDim = (upperBound - lowerBound) / 2.0f;
  center = lowerBound + maxDim;
}

void CollisionLod::ConvertToGameFormat(pugi::xml_node &out) {
  // TODO handle no collision mesh
  if (!AssimpUtil::IsZero(maxDim) || !AssimpUtil::IsZero(center)) {
    auto sizeNode = xml::AddChild(out, "size");
    xml::WriteChildXYZ("max", sizeNode, maxDim, true);
    xml::WriteChildXYZ("center", sizeNode, center, true);
  } else {
    out.remove_child("size");
    out.remove_child("size_raw");
  }
  // TODO size raw???

  if (ctx->ShouldConvertGeometry()) {
    xmfFile->WriteToFile(ctx->GetOutputPath(getName() + ".xmf"));
  }
}
}