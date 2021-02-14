#include "X4ConverterTools/model/Layer.h"
// TODO waypoints, trailemitters

#include <boost/format.hpp>
#include <utility>
#include <X4ConverterTools/util/XmlUtil.h>
#include <X4ConverterTools/util/AssimpUtil.h>

namespace model {
using namespace boost;
using util::AssimpUtil;
using util::XmlUtil;
Layer::Layer(const ConversionContext::Ptr &ctx) : AbstractElement(ctx, Qualifier) {

}

Layer::Layer(pugi::xml_node &node, const ConversionContext::Ptr &ctx, int id) : AbstractElement(ctx, Qualifier) {
  CheckXmlElement(node, "layer", false);
  layerId = id;
  auto myName = str(format("layer|%d") % layerId);
  setName(myName);
  auto lightsNode = node.child("lights");
  lights = LightsGroup(ctx, lightsNode, myName);
}

model::Layer::Layer(aiNode *node, const ConversionContext::Ptr &ctx) : AbstractElement(ctx, Qualifier) {
  ConvertFromAiNode(node);
}

aiNode *Layer::ConvertToAiNode() {
  auto result = AbstractElement::ConvertToAiNode();
  std::vector<aiNode *> children{};
  if (lights.has_value()) {
    children.push_back(lights->ConvertToAiNode());
  }
  AssimpUtil::PopulateAiNodeChildren(result, children);
  return result;
}

void Layer::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  auto myname = getName();
  std::string id = myname.substr(myname.find('|'));
  layerId = std::stoi(id);
  for (auto &child : getChildren(node)) {
    auto childName = std::string(child->mName.C_Str());
    if (childName == myname + "|lights") {
      if (lights.has_value()) {
        throw std::runtime_error("Two child lights found!");
      }
      lights = LightsGroup{ctx, child};
    }
  }
}

void Layer::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "layer") {
    throw std::runtime_error("layer was passed incorrect node to write to!");
  }
  if (lights.has_value()) {
    lights->ConvertToGameFormat(out);
  }
}

}
