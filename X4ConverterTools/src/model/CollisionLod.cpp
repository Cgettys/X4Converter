#include "X4ConverterTools/model/CollisionLod.h"
#include <iostream>
#include <boost/format.hpp>
#include <utility>

namespace model {
using util::XmlUtil;
using util::AssimpUtil;
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
  AbstractElement::ConvertFromAiNode(node);
  // Parse out the index
  size_t pos = getName().rfind("-collision");
  if (pos == std::string::npos) {
    throw std::runtime_error("this is not a collision mesh");
  }
  index = COLLISION_INDEX;
  if (ctx->ShouldConvertGeometry()) {
    CalculateSizeAndCenter(node);
    xmfFile = xmf::XmfFile::GenerateMeshFile(ctx, node, true);
  }
}

void CollisionLod::ConvertToGameFormat(pugi::xml_node &out) {
  // TODO size raw???

  // TODO handle no collision mesh
  if (!AssimpUtil::IsZero(maxDim) || !AssimpUtil::IsZero(center)) {
    auto sizeNode = XmlUtil::AddChild(out, "size");
    XmlUtil::WriteChildXYZ("max", sizeNode, maxDim, true);
    XmlUtil::WriteChildXYZ("center", sizeNode, center, true);
  } else {
    out.remove_child("size");
    out.remove_child("size_raw");
  }
  if (ctx->ShouldConvertGeometry()) {
    xmfFile->WriteToFile(ctx->fsUtil->GetOutputPath(getName() + ".xmf"));
  }
}
}