#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <utility>
#include <vector>
#include "AbstractElement.h"

namespace model {
class Connection : public AbstractElement {
 public:
  Connection(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {}

  explicit Connection(pugi::xml_node node, const ConversionContext::Ptr &ctx,
                      std::string componentName = "");

  explicit Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName = "");

  aiNode *ConvertToAiNode(pugi::xml_node intermediateXml) final;

  void ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) final;

  void ConvertToGameFormat(pugi::xml_node out) final;

  std::string getParentName();

  std::vector<Part> getParts() {
    return parts;
  }

 private:
  std::string parentName;
  std::vector<Part> parts;
};
}