#pragma once

#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"
#include "Connection.h"
#include "Layer.h"

namespace model {

class Component : public AbstractElement {
 public:
  explicit Component(ConversionContext::Ptr ctx);

  explicit Component(pugi::xml_node &node, const ConversionContext::Ptr &ctx);

  aiNode *ConvertToAiNode() override;

  void ConvertFromAiNode(aiNode *node) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

  uint32_t getNumberOfConnections();

 protected:
  std::vector<Connection> connections;
  std::vector<Layer> layers;

  void recurseOnChildren(aiNode *child, const ConversionContext::Ptr &ctx);
};

}
