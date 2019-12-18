#pragma once

#include <optional>
#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Lod.h"
#include "CollisionLod.h"
#include "VisualLod.h"
#include "Light.h"
#include <X4ConverterTools/ani/AnimFile.h>
namespace model {
class Part : public AbstractElement {
 public:
  explicit Part(ConversionContext::Ptr ctx);

  explicit Part(pugi::xml_node node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode(pugi::xml_node intermediateXml) override;

  void ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) override;

  void ConvertToGameFormat(pugi::xml_node out) override;

 private:
  bool hasRef;
  bool hasAnimation;
  std::shared_ptr<CollisionLod> collisionLod;
  std::map<int, VisualLod> lods;
  std::vector<Light> lights;
  std::optional<ani::AnimFile> animation;
  bool hasWreck;
  std::shared_ptr<CollisionLod> wreckCollisionLod;
  std::map<int, VisualLod> wreckLods;
  void handleAiLights(aiNode *node);
};
}

