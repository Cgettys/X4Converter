#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
#include <regex>
#include <utility>

namespace model {
namespace xml = util::xml;
// TODO wreck class subclassing Part?
Part::Part(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {
  hasRef = false;
  collisionLod = nullptr;
}

Part::Part(pugi::xml_node &node, const ConversionContext::Ptr &ctx) : AbstractElement(ctx) {
  if (std::string(node.name()) != "part") {
    throw std::runtime_error("XML element must be a <part> element!");
  }
  if (node.attribute("name").empty()) {
    throw std::runtime_error("Part must have a name attribute!");
  }
  hasRef = false;
  bool hasWreck = false;
  std::string wreckName;
  for (auto attr: node.attributes()) {
    auto attrName = std::string(attr.name());
    if (attrName == "ref") {
      hasRef = true;
      attrs["DO_NOT_EDIT.ref"] = attr.value();
    } else if (attrName == "name") {
      setName(attr.value());
    } else if (attrName == "wreck") {
      hasWreck = true;
      wreckName = attr.value();
      // TODO FINISH HIM
    } else {
      std::cerr << "Warning, unhandled attribute on part: " << getName() << " attribute: " << attrName
                << ". This may work fine, just a heads up ;)" << std::endl;
      attrs[attrName] = attr.value();
    }
  }

  auto lodsNode = node.child("lods");
  if (hasRef && !lodsNode.empty()) {
    throw std::runtime_error("ref should not contain lods");
  }

  // TODO common with Layer
  auto lightsNode = node.child("lights");
  if (!lightsNode.empty()) {
    for (auto &lightNode: lightsNode.children()) {
      lights.emplace_back(lightNode, ctx, getName());
    }
  }

  // TODO figure out a better way
  if (!hasRef) {
    collisionLod = std::make_unique<CollisionLod>(getName(), ctx);

    for (auto lodNode : lodsNode.children()) {
      auto lod = VisualLod(lodNode, getName(), ctx);
      lods.insert(std::pair<int, VisualLod>(lod.getIndex(), lod));
    }
  }

}

aiNode *Part::ConvertToAiNode() {
  auto *result = new aiNode(getName());
  ctx->AddMetadata(getName(), attrs);
  std::vector<aiNode *> children;
  if (!hasRef) {
    children.push_back(collisionLod->ConvertToAiNode());
    for (auto lod: lods) {
      children.push_back(lod.second.ConvertToAiNode());
    }
  }

  populateAiNodeChildren(result, children);
  return result;
}

void Part::ConvertFromAiNode(aiNode *node) {
  std::string name = node->mName.C_Str();
  setName(name);
  attrs = ctx->GetMetadataMap(name);

  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    std::string childName = child->mName.C_Str();
    // TODO check part names?
    // TODO lights better
    if (childName == name + "-lights") {
      handleAiLights(child);
    } else if (regex_match(childName, ctx->lodRegex)) {
      auto lod = VisualLod(ctx);
      lod.ConvertFromAiNode(child);
      lods.insert(std::pair<int, VisualLod>(lod.getIndex(), lod));
    } else if (regex_match(childName, ctx->collisionRegex)) {
      collisionLod = std::make_unique<CollisionLod>(ctx);
      collisionLod->ConvertFromAiNode(child);
    } else if (childName.find('*') != std::string::npos) {
      // Ignore connection, handled elsewhere
    }
  }
  // TODO more
}

void Part::handleAiLights(aiNode *node) {
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    lights.emplace_back(child, ctx);
  }
}
void Part::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "parts") {
    throw std::runtime_error("part must be appended to a parts xml element");
  }

  auto partNode = xml::AddChildByAttr(out, "part", "name", getName());


  // Note the return statement! referenced parts don't get LODS!!!
  // TODO remove if lods exist or at least error out
  if (attrs.count("DO_NOT_EDIT.ref")) {
    hasRef = true;
    auto value = attrs["DO_NOT_EDIT.ref"];
    if (partNode.attribute("ref")) {
      partNode.attribute("ref").set_value(value.c_str());
    } else {
      partNode.prepend_attribute("ref").set_value(value.c_str());
    }
    return;
  }
  for (const auto &attr : attrs) {
    xml::WriteAttr(partNode, attr.first, attr.second);
  }

  if (!lods.empty()) {
    auto lodsNode = xml::AddChild(partNode, "lods");
    collisionLod->ConvertToGameFormat(partNode);
    for (auto lod : lods) {
      lod.second.ConvertToGameFormat(lodsNode);
    }
  } else {
    partNode.remove_child("lods");
  }

  if (!lights.empty()) {
    auto lightsNode = xml::AddChild(partNode, "lights");
    for (auto light : lights) {
      light.ConvertToGameFormat(lightsNode);
    }
  }
  // TODO out more
}
}