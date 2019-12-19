#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
enum LightKind {
  arealight, omni, box
};

class Light : public AbstractElement {
 public:

  explicit Light(pugi::xml_node& node, ConversionContext::Ptr ctx, std::string parentName);

  explicit Light(aiLight *light, ConversionContext::Ptr ctx);

  aiLight *ConvertToAiLight();

  void ConvertFromAiLight(aiLight *light);

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
 public:
  explicit LightsGroup(ConversionContext::Ptr ctx);
  void ConvertFromGameFormat(pugi::xml_node &node, const std::string &parent);
  void ConvertToAiLights();
  void ConvertFromAiLights(const std::string &parent);
  void ConvertToGameFormat(pugi::xml_node &out) final;
 private:
  std::string parentName;
  std::vector<Light> lights;
};

}

