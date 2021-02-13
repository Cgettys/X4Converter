#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <utility>
#include <vector>
#include "AbstractElement.h"
#include "X4ConverterTools/util/NodeMap.h"
#include "Offset.h"

namespace model {
class Connection : public AbstractElement {
 public:
  explicit Connection(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {}

  explicit Connection(pugi::xml_node &node, const ConversionContext::Ptr &ctx);

  explicit Connection(aiNode *node, ConversionContext::Ptr ctx);

  aiNode *ConvertToAiNode() final;
  void ConvertAll(util::NodeMap &map);

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

  bool hasParent();
  void setParentName(std::string parentName);
  std::string getParentName();

 private:
  Offset offset;
  std::vector<Part> parts;
};
}