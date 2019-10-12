#include "X4ConverterTools/model/Connection.h"

#include <utility>
#include <vector>
#include <map>

using namespace util;
namespace model {

Connection::Connection(pugi::xml_node node, ConversionContext::Ptr ctx, std::string componentName)
    : AbstractElement(ctx) {

  if (!node.attribute("name")) {
    throw std::runtime_error("Unnamed connection!");
  }
  parentName = std::move(componentName);//Default to component as parent

  ReadOffset(node);

  auto partsNode = node.child("parts");
  if (partsNode) {
    for (auto child : partsNode.children()) {
      parts.emplace_back(child, ctx);
    }
  }
  for (auto attr: node.attributes()) {
    auto attrName = std::string(attr.name());
    auto value = std::string(attr.value());
    if (attrName == "name") {
      setName(value);
    } else if (attrName == "parent") {
      parentName = value;
    } else {
      attrs[attrName] = value;
    }
  }
}

Connection::Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName)
    : AbstractElement(std::move(ctx)) {
  ConvertFromAiNode(node, pugi::xml_node());
  // TODO does this do the offset?
  parentName = std::move(componentName);//Default to component as parent

}

aiNode *Connection::ConvertToAiNode(pugi::xml_node intermediateXml) {
  auto result = new aiNode("*" + getName() + "*");
  ApplyOffsetToAiNode(result);
  std::vector<aiNode *> children = attrToAiNode();
  for (auto &part : parts) {
    children.push_back(part.ConvertToAiNode(pugi::xml_node()));
  }
  populateAiNodeChildren(result, children);
  return result;
}

std::string Connection::getParentName() {
  return parentName;
}

void Connection::ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) {
  std::string tmp = node->mName.C_Str();
  setName(tmp.substr(1, tmp.size() - 2));
  // TODO check for scaling and error if cfound
  node->mTransformation.DecomposeNoScaling(offsetRot, offsetPos);

  // TODO validate attributes; better check for parts & a better solution
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    auto childName = std::string(child->mName.C_Str());
    if (childName.find('|') != std::string::npos) {
      readAiNodeChild(node, child);
    } else {
      Part part(ctx);
      part.ConvertFromAiNode(child, pugi::xml_node());
      parts.emplace(parts.end(), part);
    }
  }
}

void Connection::ConvertToGameFormat(pugi::xml_node out) {
  if (std::string(out.name()) != "connections") {
    throw std::runtime_error("parent of connection must be connections xml element");
  }
  auto node = AddChildByAttr(out, "connection", "name", getName());

  if (!parentName.empty()) {
    WriteAttr(node, "parent", parentName);
  }

  for (const auto &pair : attrs) {
    WriteAttr(node, pair.first, pair.second);
  }

  WriteOffset(node);

  if (parts.empty()) {
    return;
  }
  auto partsNode = AddChild(node, "parts");
  for (auto &part : parts) {
    part.ConvertToGameFormat(partsNode);
  }

}
}