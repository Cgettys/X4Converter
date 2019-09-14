#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"

namespace model {
class Connection : public AbstractElement {
 public:
  Connection(ConversionContext::Ptr ctx) : AbstractElement(ctx) {}

  explicit Connection(pugi::xml_node node, ConversionContext::Ptr ctx,
                      std::string componentName = "");

  explicit Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName = "");

  aiNode *ConvertToAiNode(pugi::xml_node intermediateXml) override;

  void ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) override;

  void ConvertToGameFormat(pugi::xml_node out) override;

  std::string getParentName();

  std::vector<Part> getParts() {
    return parts;
  }

 private:
  std::string parentName;
  std::vector<Part> parts;
};
}