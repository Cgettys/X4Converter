#include "X4ConverterTools/model/Light.h"
#include <boost/format.hpp>
#include <iostream>
#include <utility>

using namespace boost;
namespace model {
namespace xml = util::xml;
// TODO composition or subclass instead of this mess

Light::Light(pugi::xml_node& node, ConversionContext::Ptr ctx, std::string parentName)
    : AbstractElement(std::move(ctx)) {
  std::string tmp = str(boost::format("%1%|light|%2%") % parentName % node.attribute("name").value());
  setName(tmp);
  ReadOffset(node);
  std::string kind = node.name();
  if (kind == "arealight") {
    lightKind = arealight;
  } else if (kind == "omni") {
    lightKind = omni;
  } else if (kind == "box") {
    lightKind = box;
  } else {
    throw std::runtime_error("Unknown light type:" + kind);
  }
  color = xml::ReadAttrRGB(node);
  area = aiVector2D(node.attribute("areax").as_float(), node.attribute("areay").as_float());
  // TODO how to handle these:
  lightEffect = node.attribute("lighteffect").as_bool();
  range = node.attribute("range").as_float();
  shadowRange = node.attribute("shadowrange").as_float();
  radius = node.attribute("radius").as_float();
  spotAttenuation = node.attribute("spotattenuation").as_float();
  specularIntensity = node.attribute("specularintensity").as_float();
  trigger = node.attribute("trigger").as_bool();
  intensity = node.attribute("intensity").as_float();
  // TODO animation
}

Light::Light(aiLight &light, ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {
  ConvertFromAiLight(light);
}

aiLight Light::ConvertToAiLight() {
  aiLight result;
  result.mName = getName();
  // TODO axes?
  result.mPosition = offsetPos;

  // NB: only one value in the XML, but duplicated here to make sure it works
  result.mColorDiffuse = color;
  result.mColorSpecular = color;
  result.mColorAmbient = color;
  switch (lightKind) {
    case arealight:
      result.mType = aiLightSource_AREA;
      result.mSize = area;
      result.mUp = aiVector3D(0, 0, 1); // TODO figure this out
      result.mDirection = offsetRot.Rotate(result.mUp); // TODO checkme
      break;
    case omni:
      result.mType = aiLightSource_POINT;
      break;
    case box:
      // TODO fixme
      return result;
      result.mType = aiLightSource_AREA;
      // TODO wth is this
      result.mSize = area;
      result.mUp = aiVector3D(0, 0, 1); // TODO figure this out
      result.mDirection = offsetRot.Rotate(result.mUp); // TODO checkme

      break;
  }
  // TODO other stuff here
  return result;
}

void Light::ConvertFromAiLight(aiLight &light) {
  setName(light.mName.C_Str());
  offsetPos = light.mPosition;
  color = light.mColorSpecular;// TODO is this the best choice?
//  offsetRot = light.mDirection;
  // TODO reconstruct vector for mDirection
  switch (light.mType) {
    case aiLightSource_AREA:
      lightKind = arealight;
      area = light.mSize;
      break;
    case aiLightSource_POINT:
      lightKind = omni;
      break;
    default:
      auto err = str(format("Unknown light type from Assimp: %d") % light.mType);
      throw std::runtime_error(err);
  }

}

void Light::ConvertToGameFormat(pugi::xml_node &out) {
  auto name = getName();
  size_t pos = name.rfind("|light|");
  if (pos == std::string::npos) {
    throw std::runtime_error("light name couldn't be parsed");
  }
  name = name.substr(pos + 7);
  std::string nodeType;
  switch (lightKind) {
    case arealight:
      nodeType = "arealight";
      break;
    case omni:
      nodeType = "omni";
      break;
    case box:
      nodeType = "box";
      break;
    default:
      nodeType = "unknown";
      break;
  }

  auto lightNode = xml::AddChildByAttr(out, nodeType, "name", name);
  if (lightKind == arealight) {
    xml::WriteAttr(lightNode, "areax", area.x);
    xml::WriteAttr(lightNode, "areay", area.y);
  }

  WriteOffset(lightNode);
  xml::WriteAttrRGB(lightNode, color);
}
LightsGroup::LightsGroup(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {

}

void LightsGroup::ConvertFromGameFormat(pugi::xml_node &node, const std::string &parent) {
  parentName = parent;
  if (!node.empty()) {
    if (std::string(node.name()) != "lights") {
      throw std::runtime_error("XML element must be a <lights> element!");
    }
    for (auto lightNode: node.children()) {
      lights.emplace_back(lightNode, ctx, parentName);
    }
  }

}
void LightsGroup::ConvertToAiLights() {
  for (auto &light: lights) {
    ctx->AddLight(light.ConvertToAiLight());
  }
}
void LightsGroup::ConvertFromAiLights(const std::string &parent) {
  parentName = parent;
  auto mylights = ctx->GetLightsByParent(parentName);
  for (auto light: mylights) {
    lights.emplace_back(light, ctx);
  }
}
void LightsGroup::ConvertToGameFormat(pugi::xml_node &out) {
  // TODO is this following the correct convention we want to set?
  if (!lights.empty()) {
    auto lightsNode = util::xml::AddChild(out, "lights");
    for (auto light : lights) {
      light.ConvertToGameFormat(lightsNode);
    }
  }

}
}