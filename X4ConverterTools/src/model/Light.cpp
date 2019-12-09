#include "X4ConverterTools/model/Light.h"
#include <boost/format.hpp>
#include <iostream>
#include <utility>

using namespace boost;
namespace model {
namespace xml = util::xml;
// TODO composition or subclass isntead of this mess
Light::Light(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {

}

Light::Light(pugi::xml_node& node, ConversionContext::Ptr ctx, std::string parentName)
    : AbstractElement(std::move(ctx)) {
  std::string tmp = str(boost::format("%1%-light-%2%") % parentName % node.attribute("name").value());
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

Light::Light(aiNode *node, ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {
  ConvertFromAiNode(node, pugi::xml_node());
}

aiNode *Light::ConvertToAiNode(pugi::xml_node intermediateXml) {
  auto result = new aiLight();
  result->mName = getName();
  // TODO axes?
  result->mPosition = offsetPos;

  // NB: only one value in the XML, but duplicated here to make sure it works
  result->mColorDiffuse = color;
  result->mColorSpecular = color;
  result->mColorAmbient = color;
  auto node = new aiNode();
  node->mName = getName();
  switch (lightKind) {
    case arealight:
      result->mType = aiLightSource_AREA;
      result->mSize = area;
      result->mUp = aiVector3D(0, 0, 1); // TODO figure this out
      result->mDirection = offsetRot.Rotate(result->mUp); // TODO checkme
      break;
    case omni:
      result->mType = aiLightSource_POINT;
      break;
    case box:
      return node;
      result->mType = aiLightSource_AREA;
      // TODO wth is this
      result->mSize = area;
      result->mUp = aiVector3D(0, 0, 1); // TODO figure this out
      result->mDirection = offsetRot.Rotate(result->mUp); // TODO checkme

      break;
  }
  ctx->AddLight(result);
  // TODO other stuff here
  return node;
}

void Light::ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) {
  std::string name = node->mName.C_Str();
  setName(name);// TODO template method out this stuff?
  if (!ctx->CheckLight(name)) {
    std::cerr << "Warning, could not find light by name:" + name << std::endl;
    node->mTransformation.DecomposeNoScaling(offsetRot, offsetPos);
    return;
  }
  auto light = ctx->GetLight(name);
  offsetPos = light->mPosition;
  color = light->mColorSpecular;// TODO is this the best choice?
  // TODO reconstruct vector for mDirection
  switch (light->mType) {
    case aiLightSource_AREA:
      lightKind = arealight;
      area = light->mSize;
      break;
    case aiLightSource_POINT:
      lightKind = omni;
      break;
    default:
      auto err = str(format("Unknown light type from Assimp: %d") % light->mType);
      throw std::runtime_error(err);
  }

}

void Light::ConvertToGameFormat(pugi::xml_node out) {
  auto name = getName();
  size_t pos = name.rfind("-light-");
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

}