#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <utility>
#include <vector>
#include "AbstractElement.h"

namespace model {
class Connection : public AiNodeElement {
 public:
  explicit Connection(ConversionContext::Ptr ctx) : AiNodeElement(std::move(ctx)) {}

  explicit Connection(pugi::xml_node &node, const ConversionContext::Ptr &ctx,
                      std::string componentName = "");

  explicit Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName = "");

  aiNode *ConvertToAiNode() final;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

  std::string getParentName();

  void ConvertParts(std::map<std::string, aiNode *> map);
 private:
  std::string parentName;
  std::vector<Part> parts;
};
}