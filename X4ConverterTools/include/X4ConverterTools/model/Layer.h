#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include <vector>
#include "Light.h"

namespace model {
class Layer : public AiNodeElement {
 public:
  explicit Layer(const ConversionContext::Ptr &ctx);

  explicit Layer(pugi::xml_node &node, const ConversionContext::Ptr &ctx, int id);

  explicit Layer(aiNode *node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() override;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) override;

 protected:
  int layerId = -1;
  LightsGroup lights;
};

}
