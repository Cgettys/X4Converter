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
  static constexpr char Qualifier[] = "[P]";
  explicit Part(const ConversionContext::Ptr &ctx);

  explicit Part(pugi::xml_node &node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() override;

  void ConvertFromAiNode(aiNode *node) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

  bool HasWreck() {
    return hasAttr("wreck");
  }
  bool HasRef() {
    return hasAttr("ref");
  }
  bool HasLights() {
    return lights.has_value();
  }
 private:
  bool MatchesWreck(const std::string &childName);
  bool hasAnimation;
  std::optional<CollisionLod> collisionLod;
  std::map<int, VisualLod> lods;
  std::optional<LightsGroup> lights;
  std::optional<ani::AnimFile> animation;
  std::optional<VisualLod> wreckVisualLod;
  std::optional<CollisionLod> wreckCollisionLod;
};
}

