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
  static constexpr char Qualifier[] = "[Li]";

  explicit Light(pugi::xml_node &node, const ConversionContext::Ptr &ctx, std::string parentName);

  explicit Light(aiNode *node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() final;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

 private:
  void CheckLightKindValidity(const std::string &kind);
  Offset offset;

};

class LightsGroup : public AbstractElement {
 public:
  static constexpr char Qualifier[] = "[Lg]";
  explicit LightsGroup(const ConversionContext::Ptr &ctx, pugi::xml_node &node, const std::string &parentName);
  explicit LightsGroup(const ConversionContext::Ptr &ctx, aiNode *node);
  aiNode *ConvertToAiNode() final;
  void ConvertFromAiNode(aiNode *node) final;
  void ConvertToGameFormat(pugi::xml_node &out) final;
 private:
  std::vector<Light> lights;
};

}

