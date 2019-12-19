#include "X4ConverterTools/model/Layer.h"
// TODO waypoints, trailemitters
#include <boost/format.hpp>
#include <utility>
#include <X4ConverterTools/util/XmlUtil.h>

namespace model {
using namespace boost;
Layer::Layer(ConversionContext::Ptr ctx) : AiNodeElement(ctx), lights(ctx) {

}

Layer::Layer(pugi::xml_node &node, const ConversionContext::Ptr &ctx, int id) : AiNodeElement(ctx), lights(ctx) {
  layerId = id;
  auto myName = str(format("layer|%d") % layerId);
  setName(myName);
  auto lightsNode = node.child("lights");
  lights.ConvertFromGameFormat(node, myName);
}

model::Layer::Layer(aiNode *node, const ConversionContext::Ptr &ctx) : AiNodeElement(ctx), lights(ctx) {
  ConvertFromAiNode(node);
}

aiNode *Layer::ConvertToAiNode() {
  auto result = new aiNode();
  result->mName = getName();
  lights.ConvertToAiLights();
  return result;
}

void Layer::ConvertFromAiNode(aiNode *node) {
  std::string myname{node->mName.C_Str()};
  setName(myname);
  std::string id = myname.substr(myname.find('|'));
  layerId = std::stoi(id);
  lights.ConvertFromAiLights(myname);
}


void Layer::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "layer") {
    throw std::runtime_error("layer was passed incorrect node to write to!");
  }
  lights.ConvertToGameFormat(out);
}

}
