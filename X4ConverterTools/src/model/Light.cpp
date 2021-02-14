#include "X4ConverterTools/model/Light.h"
#include <boost/format.hpp>
#include <iostream>
#include <utility>
#include <X4ConverterTools/util/AssimpUtil.h>

using namespace boost;
namespace model {
using util::XmlUtil;
using util::AssimpUtil;
Light::Light(pugi::xml_node &node, const ConversionContext::Ptr &ctx, std::string parentName)
    : AbstractElement(ctx, Light::Qualifier), offset(node) {
  std::string tmp = str(boost::format("%1%|%2%") % parentName % node.attribute("name").value());
  setName(tmp);
  std::string kind = node.name();
  CheckLightKindValidity(kind);
  setAttr("kind", kind);
  ProcessAttributes(node);
  // TODO animation
}

Light::Light(aiNode *node, const ConversionContext::Ptr &ctx) : AbstractElement(ctx, Light::Qualifier) {
  ConvertFromAiNode(node);
}

aiNode *Light::ConvertToAiNode() {
  auto result = AbstractElement::ConvertToAiNode();
  offset.WriteAiNode(result);
  // TODO other stuff here
  return result;
}

void Light::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  auto kind = getAttr("kind");;
  CheckLightKindValidity(kind);
  offset.ReadAiNode(node);
}

void Light::ConvertToGameFormat(pugi::xml_node &out) {
  auto name = getName();
  size_t pos = name.rfind('|');
  if (pos == std::string::npos) {
    throw std::runtime_error("light name couldn't be parsed");
  }
  name = name.substr(pos + 1);
  auto lightNode = XmlUtil::AddChildByAttr(out, getAttr("kind"), "name", name);
  WriteAttrs(lightNode, ExcludePredicate("kind"));
  offset.WriteXml(lightNode);
}
void Light::CheckLightKindValidity(const std::string &kind) {
  if (!(kind == "arealight" || kind == "omni" || kind == "box")) {
    throw std::runtime_error("Unknown light type:" + kind);
  }
}
LightsGroup::LightsGroup(const ConversionContext::Ptr &ctx, pugi::xml_node &node, const std::string &parentName)
    : AbstractElement(ctx, LightsGroup::Qualifier) {
  CheckXmlElement(node, "lights", false);
  setName(parentName + "|lights");
  for (auto lightNode: node.children()) {
    lights.emplace_back(lightNode, ctx, parentName);
  }
}

LightsGroup::LightsGroup(const ConversionContext::Ptr &ctx, aiNode *node) : AbstractElement(ctx,
                                                                                            LightsGroup::Qualifier) {
  ConvertFromAiNode(node);
}
aiNode *LightsGroup::ConvertToAiNode() {
  auto result = AbstractElement::ConvertToAiNode();
  std::vector<aiNode *> children{};
  for (auto &light: lights) {
    children.push_back(light.ConvertToAiNode());
  }
  AssimpUtil::PopulateAiNodeChildren(result, children);
  return result;
}
void LightsGroup::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  for (auto &child : getChildren(node)) {
    lights.emplace_back(child, ctx);
  }
}
void LightsGroup::ConvertToGameFormat(pugi::xml_node &out) {
  // TODO is this following the correct convention we want to set?
  if (!lights.empty()) {
    auto lightsNode = util::XmlUtil::AddChild(out, "lights");
    for (auto light : lights) {
      light.ConvertToGameFormat(lightsNode);
    }
  }

}

}