#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
enum LightKind {
  arealight, omni, box
};

class Light : public AbstractElement {
 public:
  explicit Light(ConversionContext::Ptr ctx);

  explicit Light(pugi::xml_node& node, ConversionContext::Ptr ctx, std::string parentName);

  explicit Light(aiNode *node, ConversionContext::Ptr ctx);

  aiNode *ConvertToAiNode() final;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

 private:
  LightKind lightKind;
  aiColor3D color;
  aiVector2D area;
  bool lightEffect;
  float range;
  float shadowRange;
  float radius;
  float spotAttenuation;
  float specularIntensity;
  bool trigger; // TODO confirm bools
  float intensity;
};

class LightsGroup : public AbstractElement {
  LightsGroup(pugi::xml_node &node, ConversionContext::Ptr ctx, std::string parentName);
  aiNode *ConvertToAiNode() final;
  void ConvertFromAiNode(aiNode *node) final;
  void ConvertToGameFormat(pugi::xml_node &out) final;
};

}

