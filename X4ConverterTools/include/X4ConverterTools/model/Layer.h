#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include <vector>
#include "Light.h"

namespace model {
/*
Models the layer element and its children. waypoints, trailemitters, and sounds are not currently handled
<layer>
    <waypoints/>
    <lights/>
    <trailemitters/>
    <sounds/>
</layer>
*/

class Layer : public AbstractElement {
 public:
  static constexpr char Qualifier[] = "[La]";
  explicit Layer(const ConversionContext::Ptr &ctx);

  explicit Layer(pugi::xml_node &node, const ConversionContext::Ptr &ctx, int id);

  explicit Layer(aiNode *node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() override;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) override;

 protected:
  int layerId = -1;
  std::optional<LightsGroup> lights;
};

}
