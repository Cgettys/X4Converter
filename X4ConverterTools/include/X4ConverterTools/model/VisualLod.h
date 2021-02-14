#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

class VisualLod : public Lod {
 public:
  explicit VisualLod(ConversionContext::Ptr ctx);

  VisualLod(int index, std::string partName, const ConversionContext::Ptr &ctx);

  void ConvertFromAiNode(aiNode *node) override;

  void ConvertToGameFormat(pugi::xml_node &out) override;

  static bool MatchesPattern(std::string name);
};

}


