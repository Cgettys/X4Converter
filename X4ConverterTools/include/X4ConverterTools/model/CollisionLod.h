#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include "Lod.h"

namespace model {

const static int COLLISION_INDEX = -1;

class CollisionLod : public Lod {
 public:
  explicit CollisionLod(ConversionContext::Ptr ctx);

  explicit CollisionLod(std::string partName, const ConversionContext::Ptr &ctx);

  void ConvertFromAiNode(aiNode *node, pugi::xml_node &intermediateXml) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

 private:

  void CalculateSizeAndCenter(aiNode *pCollisionNode);

  aiVector3D maxDim;
  aiVector3D center;
};

}


