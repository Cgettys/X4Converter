#include "X4ConverterTools/model/Connection.h"
#include <boost/algorithm/string.hpp>
#include <utility>
#include <vector>
#include <map>

using namespace util;
namespace model {
using util::XmlUtil;

Connection::Connection(pugi::xml_node &node, const ConversionContext::Ptr &ctx) :
    AbstractElement(ctx), offset(node) {
  // TODO document that this gets the name as well
  CheckXmlElement(node, "connection");
  setName("*" + getName() + "*");

  auto partsNode = node.child("parts");
  if (partsNode) {
    for (auto child : partsNode.children()) {
      parts.emplace_back(child, ctx);
    }
  }
  ProcessAttributes(node);
}

Connection::Connection(aiNode *node, ConversionContext::Ptr ctx) :
    AbstractElement(std::move(ctx)) {
  ConvertFromAiNode(node);
}

aiNode *Connection::ConvertToAiNode() {
  auto result = AbstractElement::ConvertToAiNode();
  offset.WriteAiNode(result);
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

bool Connection::hasParent() {
  return hasAttr("parent");
}
void Connection::setParentName(std::string parentName) {
  setAttr("parent", parentName);
}
std::string Connection::getParentName() {
  return getAttr("parent");
}

void Connection::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  setName(getName().substr(1, getName().size() - 2));

  offset.ReadAiNode(node);

  // TODO validate attributes; better check for parts & a better solution
  for (auto &child : getChildren(node)) {
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
  auto outName = getName();
  auto node = XmlUtil::AddChildByAttr(out, "connection", "name", outName);
  WriteAttrs(node);
  offset.WriteXml(node);

  if (parts.empty()) {
    return;
  }
  auto partsNode = XmlUtil::AddChild(node, "parts");
  for (auto &part : parts) {
    part.ConvertToGameFormat(partsNode);
  }

}
}