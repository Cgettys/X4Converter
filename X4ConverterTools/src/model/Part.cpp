#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
#include <regex>
#include <utility>

namespace model {
using util::XmlUtil;
using util::AssimpUtil;

// TODO wreck class subclassing Part?
Part::Part(const ConversionContext::Ptr &ctx) : AbstractElement(ctx, Qualifier) {
}

Part::Part(pugi::xml_node &node, const ConversionContext::Ptr &ctx) : AbstractElement(ctx, Qualifier) {
  CheckXmlElement(node, "part");
  ProcessAttributes(node);

  auto lodsNode = node.child("lods");
  if (HasRef() && !lodsNode.empty()) {
    throw std::runtime_error("ref should not contain lods");
  }

  auto lightsNode = node.child("lights");
  if (!lightsNode.empty()) {
    lights = LightsGroup(ctx, lightsNode, getName());
  }

  // TODO figure out a better way
  if (!HasRef()) {
    for (auto lodNode : lodsNode.children()) {
      if (std::string(lodNode.name()) != "lod") {
        throw std::runtime_error("XML element must be a <lod> element!");
      }
      if (lodNode.attribute("index").empty()) {
        throw std::runtime_error("VisualLod must have an index attribute!");
      }
      auto lodIndex = lodNode.attribute("index").as_int();
      auto lod = VisualLod(lodIndex, getName(), ctx);
      auto result = lods.insert(std::pair<int, VisualLod>(lodIndex, lod));
      if (!result.second) {
        throw std::runtime_error("Duplicate Lod index read");
      }
    }
    collisionLod = CollisionLod(getName(), ctx);
    if (HasWreck()) {
      // TODO verify this: wrecks apparently only get one lod
      auto wreckName = getAttr("wreck");
      wreckVisualLod = VisualLod(0, wreckName, ctx);
      wreckCollisionLod = CollisionLod(wreckName, ctx);
    }
  }

}

aiNode *Part::ConvertToAiNode() {
  auto result = AbstractElement::ConvertToAiNode();
  std::vector<aiNode *> children{};
  if (!HasRef()) {
    children.push_back(collisionLod->ConvertToAiNode());
    for (auto lod: lods) {
      children.push_back(lod.second.ConvertToAiNode());
    }
    if (HasWreck()) {
      children.push_back(wreckVisualLod->ConvertToAiNode());
      children.push_back(wreckCollisionLod->ConvertToAiNode());
    }
    if (HasLights()) {
      children.push_back(lights->ConvertToAiNode());
    }
    AssimpUtil::PopulateAiNodeChildren(result, children);
  }
  return result;
}

bool Part::MatchesWreck(const std::string &childName) {
  if (HasWreck()) {
    std::string wreckName = getAttr("wreck");;
    if (wreckName.empty()) {
      throw std::runtime_error("Wreck name cannot be empty!");
    }
    if (childName.find(wreckName) != std::string::npos) {
      return true;
    }
  }
  return false;
}
void Part::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  // TODO handle children if ref?
  for (auto &child : getChildren(node)) {
    std::string childName = child->mName.C_Str();
    // TODO check part names?
    if (matchesQualifier(childName, Light::Qualifier)) {
      if (HasLights()) {
        throw std::runtime_error("Found two lights for part!");
      } else {
        lights = LightsGroup(ctx, child);
      }
    } else if (VisualLod::MatchesPattern(childName)) {
      auto lod = VisualLod(ctx);
      lod.ConvertFromAiNode(child);
      // The part name may be contained in the wreck name but usually not the reverse, so we check for the wreck name
      if (MatchesWreck(childName)) {
        if (wreckVisualLod.has_value()) {
          throw std::runtime_error("Found two wreck visual lods!");
        }
        wreckVisualLod = lod;
      } else {
        lods.insert(std::pair<int, VisualLod>(lod.getIndex(), lod));
      }
    } else if (CollisionLod::MatchesPattern(childName)) {
      auto lod = CollisionLod(ctx);
      lod.ConvertFromAiNode(child);
      if (MatchesWreck(childName)) {
        if (wreckCollisionLod.has_value()) {
          throw std::runtime_error("Found two wreck collision lods!");
        }
        wreckCollisionLod = lod;
      } else {
        if (collisionLod.has_value()) {
          throw std::runtime_error("Found two collision lods for part!");
        }
        collisionLod = lod;
      }

    } else if (childName.find('*') != std::string::npos) {
      // Ignore connection, handled elsewhere
    }
  }
  // TODO more
}

void Part::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "parts") {
    throw std::runtime_error("part must be appended to a parts xml element");
  }

  auto partNode = util::XmlUtil::XmlUtil::AddChildByAttr(out, "part", "name", getName());


  // Note the return statement! referenced parts don't get LODS!!!
  // TODO remove if lods exist or at least error out
//  if (HasRef()) {
//    auto value = getAttr("ref");;
//    if (partNode.attribute("ref")) {
//      partNode.attribute("ref").set_value(value.c_str());
//    } else {
//      partNode.prepend_attribute("ref").set_value(value.c_str());
//    }
//    return;
//  }
  WriteAttrs(partNode);
  // TODO warn if missing middle lod, etc?

  if (!lods.empty()) {
    auto lodsNode = XmlUtil::AddChild(partNode, "lods");
    collisionLod->ConvertToGameFormat(partNode);
    for (auto lod : lods) {
      lod.second.ConvertToGameFormat(lodsNode);
    }
  } else {
    partNode.remove_child("lods");
  }
  if (HasLights()) {
    lights->ConvertToGameFormat(out);
  }
  // TODO out more
}
}