#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
#include <regex>
#include <utility>

namespace model {
using util::XmlUtil;

// TODO wreck class subclassing Part?
Part::Part(const ConversionContext::Ptr &ctx) : AiNodeElement(ctx), lights(ctx) {
}

Part::Part(pugi::xml_node &node, const ConversionContext::Ptr &ctx) : AiNodeElement(ctx), lights(ctx) {
  if (std::string(node.name()) != "part") {
    throw std::runtime_error("XML element must be a <part> element!");
  }
  if (node.attribute("name").empty()) {
    throw std::runtime_error("Part must have a name attribute!");
  }
  for (auto attr: node.attributes()) {
    auto attrName = std::string(attr.name());
    if (attrName == "ref") {
      attrs["ref"] = attr.value();
    } else if (attrName == "name") {
      setName(attr.value());
    } else if (attrName == "wreck") {
      attrs["wreck"] = attr.value();
      // TODO there has to be a more elegant way of handling this
    } else {
      std::cerr << "Warning, unhandled attribute on part: " << getName() << " attribute: " << attrName
                << ". This may work fine, just a heads up ;)" << std::endl;
      attrs[attrName] = attr.value();
    }
  }

  auto lodsNode = node.child("lods");
  if (HasRef() && !lodsNode.empty()) {
    throw std::runtime_error("ref should not contain lods");
  }

  auto lightsNode = node.child("lights");
  lights.ConvertFromGameFormat(lightsNode, getName());

  // TODO figure out a better way
  if (!HasRef()) {
    for (auto lodNode : lodsNode.children()) {
      if (std::string(lodNode.name()) != "lod") {
        throw std::runtime_error("XML element must be a <lod> element!");
      }
      if (lodNode.attribute("index").empty()) {
        throw std::runtime_error("VisualLod must have an index attribute!");
      }
      auto lodIndex = node.attribute("index").as_int();
      auto lod = VisualLod(lodIndex, getName(), ctx);
      lods.insert(std::pair < int, VisualLod > (lodIndex, lod));
    }
    collisionLod = CollisionLod(getName(), ctx);
    if (HasWreck()) {
      // TODO verify this: wrecks apparently only get one lod
      wreckVisualLod = VisualLod(0, attrs["wreck"], ctx);
      wreckCollisionLod = CollisionLod(attrs["wreck"], ctx);
    }
  }

}

aiNode *Part::ConvertToAiNode() {
  auto *result = new aiNode;
  result->mName = aiString{getName()};
  ctx->AddMetadata(getName(), attrs);
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
    populateAiNodeChildren(result, children);
    lights.ConvertToAiLights();
  }
  return result;
}

void Part::ConvertFromAiNode(aiNode *node) {
  std::string name = node->mName.C_Str();
  setName(name);
  attrs = ctx->GetMetadataMap(name);
  lights.ConvertFromAiLights(name);

  // TODO handle children if ref?

  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    std::string childName = child->mName.C_Str();
    // TODO check part names?
    // TODO lights better
    if (regex_match(childName, ctx->lodRegex)) {
      auto lod = VisualLod(ctx);
      lod.ConvertFromAiNode(child);
      // The part name may be contained in the wreck name but usually not the reverse, so we check for the wreck name
      if (HasWreck() && lod.getName().find(attrs["wreck"])) {
        if (wreckVisualLod.has_value()) {
          throw std::runtime_error("Found two wreck visual lods!");
        }
        wreckVisualLod = lod;
      } else {
        lods.insert(std::pair < int, VisualLod > (lod.getIndex(), lod));
      }
    } else if (regex_match(childName, ctx->collisionRegex)) {
      auto lod = CollisionLod(ctx);
      lod.ConvertFromAiNode(child);
      if (HasWreck() && lod.getName().find(attrs["wreck"])) {
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
  if (HasRef()) {
    auto value = attrs["ref"];
    if (partNode.attribute("ref")) {
      partNode.attribute("ref").set_value(value.c_str());
    } else {
      partNode.prepend_attribute("ref").set_value(value.c_str());
    }
    return;
  }
  for (const auto &attr : attrs) {
    XmlUtil::WriteAttr(partNode, attr.first, attr.second);
  }

  if (!lods.empty()) {
    auto lodsNode = XmlUtil::AddChild(partNode, "lods");
    collisionLod->ConvertToGameFormat(partNode);
    for (auto lod : lods) {
      lod.second.ConvertToGameFormat(lodsNode);
    }
  } else {
    partNode.remove_child("lods");
  }

  lights.ConvertToGameFormat(out);
  // TODO out more
}
}