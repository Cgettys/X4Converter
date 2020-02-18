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
class Part : public AiNodeElement {
 public:
  explicit Part(const ConversionContext::Ptr &ctx);

  explicit Part(pugi::xml_node &node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() override;

  void ConvertFromAiNode(aiNode *node) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

  bool HasWreck() {
    return attrs.count("wreck") > 0;
  }
  bool HasRef() {
    return attrs.count("ref") > 0;
  }
 private:
  bool MatchesWreck(const std::string &childName);
  bool hasAnimation;
  std::optional<CollisionLod> collisionLod;
  std::map<int, VisualLod> lods;
  LightsGroup lights;
  std::optional<ani::AnimFile> animation;
  std::optional<VisualLod> wreckVisualLod;
  std::optional<CollisionLod> wreckCollisionLod;
};
}

