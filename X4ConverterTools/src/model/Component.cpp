#include "X4ConverterTools/model/Component.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <utility>
#include <X4ConverterTools/model/NodeMap.h>

using namespace boost::algorithm;
namespace model {
using util::XmlUtil;

Component::Component(ConversionContext::Ptr ctx) : AiNodeElement(std::move(ctx)) {}

Component::Component(pugi::xml_node &node, const ConversionContext::Ptr &ctx) : AiNodeElement(ctx) {
  CheckXmlNode(node, "component");
  if (!node.child("source")) {
    std::cerr << "source directory not specified" << std::endl;
  } else {
    std::string src = node.child("source").attribute("geometry").value();
    if (src.empty()) {
      throw runtime_error("Source directory for geometry must be specified!");
    }
    attrs["source"] = src;
    ctx->SetSourcePathSuffix(src);
  }

  for (auto attr: node.attributes()) {
    auto attrName = std::string(attr.name());
    auto attrValue = std::string(attr.value());
    if (attrName == "name") {
      setName(attrValue);
    } else {
      attrs[attrName] = attrValue;
    }
  }
  auto connectionsNode = node.child("connections");
  if (connectionsNode.empty()) {
    std::cerr << "Warning, could not find any <connection> nodes!" << std::endl;
  }
  for (auto connectionNode : connectionsNode.children()) {
    connections.emplace_back(connectionNode, ctx, getName());
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

class ParentMap {

};
aiNode *Component::ConvertToAiNode() {
  NodeMap nodes;
  auto result = nodes.CreateNode(getName());
  ctx->AddMetadata(getName(), attrs);
  // Handle layers
  nodes.CreateNode("layers");
  for (auto &layer : layers) {
    auto *layerAiNode = layer.ConvertToAiNode();
    nodes.AddNode(layerAiNode);
    nodes.MakeParent("layers", layer.getName());
  }
  // Convert all the nodes
  for (auto &conn : connections) {
    std::string connName = conn.getName();
    conn.ConvertAll(nodes);
    nodes.MakeParent(conn.getParentName(), conn.getName());
  }
  // Now to unflatten everything
  nodes.PopulateChildren();

  // Now double check that we didn't miss anything
  for (auto &conn: connections) {
    auto connNode = nodes.GetNode(conn.getName());
    if (connNode->mParent == nullptr) {
      throw std::runtime_error("connection" + conn.getName() + "lost its parent");
    }
  }

  auto rootChildren = new aiNode *[1];
  rootChildren[0] = result;
  auto fakeRoot = new aiNode("ROOT");
  fakeRoot->addChildren(1, rootChildren);
  delete[] rootChildren;
  return fakeRoot;
}

void Component::ConvertFromAiNode(aiNode *node) {
  setName(node->mName.C_Str());
  attrs = ctx->GetMetadataMap(getName());
  for (int i = 0; i < node->mNumChildren; i++) {
    auto child = node->mChildren[i];
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
  for (int i = 0; i < tgt->mNumChildren; i++) {
    auto child = tgt->mChildren[i];
    std::string childName = child->mName.C_Str();
    if (childName.find('*') != std::string::npos) {
      if (is_connection) {
        throw std::runtime_error("connection cannot have a connection as a parent!");
      }
      connections.emplace_back(child, ctx, tgtName);
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
  for (const auto &attr : attrs) {
    if (attr.first == "source") {
      XmlUtil::AddChildByAttr(compNode, "source", "geometry", attr.second);
      // TODO compare to output path and confirm if wrong
      ctx->SetSourcePathSuffix(attr.second);
    } else {
      XmlUtil::WriteAttr(compNode, attr.first, attr.second);
    }
  }
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