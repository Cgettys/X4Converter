#include "X4ConverterTools/model/Layer.h"
// TODO waypoints, trailemitters
#include <boost/format.hpp>
#include <utility>
#include <X4ConverterTools/util/XmlUtil.h>

namespace model {
using namespace boost;
Layer::Layer(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {

}

Layer::Layer(pugi::xml_node node, const ConversionContext::Ptr &ctx, int id) : AbstractElement(ctx) {
  layerId = id;
  setName(str(format("layer%d") % layerId));
  auto lightsNode = node.child("lights");
  if (!lightsNode.empty()) {
    for (auto lightNode: lightsNode.children()) {
      lights.emplace_back(lightNode, ctx, getName());
    }
  }
}

model::Layer::Layer(aiNode *node, ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {
  ConvertFromAiNode(node, pugi::xml_node());
}

aiNode *Layer::ConvertToAiNode(pugi::xml_node intermediateXml) {
  auto result = new aiNode();
  result->mName = getName();

  auto lightResult = new aiNode();
  lightResult->mName = getName() + "-lights";
  // TODO should really add a Lights object or something
  std::vector<aiNode *> lightChildren;
  for (auto light: lights) {
    lightChildren.push_back(light.ConvertToAiNode(pugi::xml_node()));
  }
  populateAiNodeChildren(lightResult, lightChildren);

  std::vector<aiNode *> children;
  children.push_back(lightResult);
  populateAiNodeChildren(result, children);
  return result;
}

void Layer::ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) {
  std::string name = node->mName.C_Str();
  setName(name);
  // TODO abstract out some of the shared logic with Component
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    std::string childName = child->mName.C_Str();
    if (childName == name + "-lights") {
      handleAiLights(child);
      continue;
    } else {
      throw std::runtime_error("Unexpected child name under layer: " + childName);
    }
  }

}

void Layer::handleAiLights(aiNode *node) {
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    lights.emplace_back(child, ctx);
  }
}

void Layer::ConvertToGameFormat(pugi::xml_node out) {
  if (out.name() != "layer") {
    throw std::runtime_error("layer was passed incorrect node to write to!");
  }
  if (!lights.empty()) {
    auto lightsNode = util::xml::AddChild(out, "lights");
    for (auto light : lights) {
      light.ConvertToGameFormat(lightsNode);
    }
  }
}

}
