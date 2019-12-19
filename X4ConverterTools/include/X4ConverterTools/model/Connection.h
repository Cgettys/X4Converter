#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <utility>
#include <vector>
#include "AbstractElement.h"

namespace model {
class Connection : public AiNodeElement {
 public:
  Connection(ConversionContext::Ptr ctx) : AiNodeElement(std::move(ctx)) {}

  explicit Connection(pugi::xml_node &node, const ConversionContext::Ptr &ctx,
                      std::string componentName = "");

  explicit Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName = "");

  aiNode *ConvertToAiNode() final;

  void ConvertFromAiNode(aiNode *node) final;

  void ConvertToGameFormat(pugi::xml_node &out) final;

  std::string getParentName();

  std::vector<Part> getParts() {
    return parts;
  }

 private:
  std::string parentName;
  std::vector<Part> parts;
};
}