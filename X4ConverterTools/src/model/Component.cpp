#include "X4ConverterTools/model/Component.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <utility>
#include <X4ConverterTools/util/NodeMap.h>

using namespace boost::algorithm;
namespace model {
using util::XmlUtil;

Component::Component(ConversionContext::Ptr ctx) : AbstractElement(std::move(ctx)) {}

Component::Component(pugi::xml_node &node, const ConversionContext::Ptr &ctx) : AbstractElement(ctx) {
  CheckXmlElement(node, "component");
  if (!node.child("source")) {
    std::cerr << "source directory not specified" << std::endl;
  } else {
    std::string src = node.child("source").attribute("geometry").value();
    if (src.empty()) {
      throw runtime_error("Source directory for geometry must be specified!");
    }
    setAttr("source", src);;
    ctx->fsUtil->SetSourcePathSuffix(src);
  }
  ProcessAttributes(node);
  auto connectionsNode = node.child("connections");
  if (connectionsNode.empty()) {
    std::cerr << "Warning, could not find any <connection> nodes!" << std::endl;
  }
  for (auto connectionNode : connectionsNode.children()) {
    connections.emplace_back(connectionNode, ctx);
  }

  auto layersNode = node.child("layers");
  if (layersNode.empty()) {
    std::cerr << "Warning, <layers> node not found" << std::endl;
  }
  // Index the layers so we can reassemble them in the same order. Not sure it really matters, but this makes things
  // more consistent
  int layerIndex = 0;
  for (auto layerNode : layersNode.children()) {
    layers.emplace_back(layerNode, ctx, layerIndex);
    ++layerIndex;
  }
}

aiNode *Component::ConvertToAiNode() {
  util::NodeMap nodes;
  auto result = nodes.CreateNode(getName());
  // Handle layers
  auto layersNode = nodes.CreateNode("layers");
  nodes.MakeParent(getName(), "layers");
  for (auto &layer : layers) {
    auto *layerAiNode = layer.ConvertToAiNode();
    nodes.AddNode(layerAiNode);
    nodes.MakeParent("layers", layer.getName());
  }
  // Convert all the nodes
  for (auto &conn : connections) {
    std::string connName = conn.getName();
    conn.ConvertAll(nodes);
    auto parentName = conn.hasParent() ? conn.getParentName() : getName();
    nodes.MakeParent(parentName, conn.getName());
  }
  // Now to unflatten everything
  nodes.PopulateChildren();

  auto rootChildren = new aiNode *[1];
  rootChildren[0] = result;
  auto fakeRoot = new aiNode("ROOT");
  fakeRoot->addChildren(1, rootChildren);
  delete[] rootChildren;
  return fakeRoot;
}

void Component::ConvertFromAiNode(aiNode *node) {
  AbstractElement::ConvertFromAiNode(node);
  for (auto &child: getChildren(node)) {
    std::string childName = child->mName.C_Str();
    if (childName.find('*') == std::string::npos) {
      std::cerr << "Warning, possible non-component directly under root, ignoring: " << childName
                << std::endl;
    } else if (starts_with(childName, "layer")) {
      layers.emplace_back(child, ctx);
    } else {
      connections.emplace_back(child, ctx);
      recurseOnChildren(child, ctx);
    }
  }
}

void Component::recurseOnChildren(aiNode *tgt, const ConversionContext::Ptr &ctx) {
  std::string tgtName = tgt->mName.C_Str();
  bool is_connection = tgtName.find('*') != std::string::npos;
  for (auto &child : getChildren(tgt)) {
    std::string childName = child->mName.C_Str();
    if (childName.find('*') != std::string::npos) {
      if (is_connection) {
        throw std::runtime_error("connection cannot have a connection as a parent!");
      }
      auto result = connections.emplace_back(child, ctx);
      result.setParentName(tgtName);
    }
    recurseOnChildren(child, ctx);
  }

}

void Component::ConvertToGameFormat(pugi::xml_node &out) {
  // TODO asset.xmf?
  if (std::string(out.name()) != "components") {
    throw std::runtime_error("Component should be under components element");
  }
  auto compNode = XmlUtil::AddChildByAttr(out, "component", "name", getName());
  auto connsNode = XmlUtil::AddChild(compNode, "connections");
  auto src = getAttr("source");
  XmlUtil::AddChildByAttr(compNode, "source", "geometry", src);
  // TODO compare to output path and confirm if wrong
  ctx->fsUtil->SetSourcePathSuffix(src);
  WriteAttrs(compNode, ExcludePredicate("source"));
  for (auto &conn : connections) {
    conn.ConvertToGameFormat(connsNode);
  }
  auto layersNode = XmlUtil::AddChild(compNode, "layers");
  for (auto &layer : layers) {
    layer.ConvertToGameFormat(layersNode);
  }
}

uint32_t Component::getNumberOfConnections() {
  return boost::numeric_cast<uint32_t>(connections.size());
}
}