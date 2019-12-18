#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

class VisualLod : public Lod {
 public:
  explicit VisualLod(ConversionContext::Ptr ctx);

  VisualLod(pugi::xml_node node, std::string partName, const ConversionContext::Ptr &ctx);

  void ConvertFromAiNode(aiNode *node, pugi::xml_node &intermediateXml) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

 private:

};

}


