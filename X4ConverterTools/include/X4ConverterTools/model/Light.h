#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Offset.h"

namespace model {
enum LightKind {
  arealight, omni, box
};

class Light : public AbstractElement {
 public:

  explicit Light(pugi::xml_node &node, ConversionContext::Ptr ctx, std::string parentName);

  explicit Light(aiNode *node, ConversionContext::Ptr ctx);

  aiNode *ConvertToAiNode() final;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

 private:
  void CheckLightKindValidity(const std::string &kind);
  Offset offset;

};

class LightsGroup : public AbstractElement {
 public:
  explicit LightsGroup(ConversionContext::Ptr ctx, pugi::xml_node &node, const std::string &parentName);
  explicit LightsGroup(ConversionContext::Ptr ctx, aiNode *node);
  aiNode *ConvertToAiNode() final;
  void ConvertFromAiNode(aiNode *node) final;
  void ConvertToGameFormat(pugi::xml_node &out) final;
 private:
  std::vector<Light> lights;
};

}

