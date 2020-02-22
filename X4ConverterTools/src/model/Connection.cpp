#include "X4ConverterTools/model/Connection.h"

#include <utility>
#include <vector>
#include <map>

using namespace util;
namespace model {
using util::XmlUtil;

Connection::Connection(pugi::xml_node &node, const ConversionContext::Ptr &ctx, std::string componentName)
    : AiNodeElement(ctx) {
  CheckXmlNode(node, "connection");
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
      setName("*" + value + "*");
    } else if (attrName == "parent") {
      parentName = value;
    } else {
      attrs[attrName] = value;
    }
  }
}

Connection::Connection(aiNode *node, ConversionContext::Ptr ctx, std::string componentName)
    : AiNodeElement(std::move(ctx)) {
  ConvertFromAiNode(node);
  // TODO does this do the offset?
  parentName = std::move(componentName);//Default to component as parent

}

aiNode *Connection::ConvertToAiNode() {
  auto result = new aiNode(getName());
  ctx->AddMetadata(getName(), attrs);
  ApplyOffsetToAiNode(result);
  std::vector<aiNode *> children{};
  for (auto &part : parts) {
    children.push_back(part.ConvertToAiNode());
  }
  AssimpUtil::PopulateAiNodeChildren(result, children);
  return result;
}
// TODO figure out how to get rid of this special handling
void Connection::ConvertAll(NodeMap &nodes) {
  auto result = ConvertToAiNode();
  nodes.AddNode(result);
  for (auto &part : parts) {
    std::string partName = part.getName();
    auto partNode = result->FindNode(partName.c_str());
    if (partNode == nullptr) {
      throw std::runtime_error(
          "Something has gone horribly wrong! Could not find node for part: " + partName + " under connection: "
              + getName());
    }
    nodes.AddNode(partNode);
  }

}

std::string Connection::getParentName() {
  return parentName;
}

void Connection::ConvertFromAiNode(aiNode *node) {
  std::string tmp = node->mName.C_Str();
  setName(tmp.substr(1, tmp.size() - 2));
  attrs = ctx->GetMetadataMap(tmp);
  // TODO check for scaling and error if found
  node->mTransformation.DecomposeNoScaling(offsetRot, offsetPos);

  // TODO validate attributes; better check for parts & a better solution
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
    auto childName = std::string(child->mName.C_Str());
    Part part(ctx);
    part.ConvertFromAiNode(child);
    parts.emplace(parts.end(), part);
  }
}

void Connection::ConvertToGameFormat(pugi::xml_node &out) {
  if (std::string(out.name()) != "connections") {
    throw std::runtime_error("parent of connection must be connections xml element");
  }
  auto node = XmlUtil::AddChildByAttr(out, "connection", "name", getName());

  if (!parentName.empty()) {
    XmlUtil::WriteAttr(node, "parent", parentName);
  }

  for (const auto &pair : attrs) {
    XmlUtil::WriteAttr(node, pair.first, pair.second);
  }

  WriteOffset(node);

  if (parts.empty()) {
    return;
  }
  auto partsNode = XmlUtil::AddChild(node, "parts");
  for (auto &part : parts) {
    part.ConvertToGameFormat(partsNode);
  }

}
}