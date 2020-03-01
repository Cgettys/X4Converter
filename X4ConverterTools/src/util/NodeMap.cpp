#include "X4ConverterTools/util/NodeMap.h"
namespace util {
using util::AssimpUtil;
aiNode *NodeMap::CreateNode(const std::string &name) {
  auto result = new aiNode;
  result->mName = name;
  AddNode(result);
  return result;
}

void NodeMap::AddNode(aiNode *node) {
  if (node == nullptr) {
    throw std::runtime_error("Null aiNode passed to NodeMap!");
  }
  std::string nodeName = node->mName.C_Str();
  if (nodes.count(nodeName)) {
    throw std::runtime_error("Duplicate key is not allowed! Two instances of name: " + nodeName);
  }
  nodes[nodeName] = node;
}
bool NodeMap::HasNode(const std::string &name) {
  return nodes.count(name) > 0;
}
aiNode *NodeMap::GetNode(const std::string &name) {
  if (!HasNode(name)) {
    throw std::runtime_error("NodeMap does not have node: " + name);
  }
  return nodes[name];
}
void NodeMap::Print() {
  AssimpUtil::PrintAiNodeMap(nodes);
}
// TODO ensure multiple parentage is disallowed
void NodeMap::MakeParent(const std::string &parentName, const std::string &childName) {
  if (parentMap.count(parentName) == 0) {
    parentMap[parentName] = std::vector<std::string>();
  }
  if (!HasNode(childName)) {
    throw std::runtime_error("Child has not been added to NodeMap!");
  }
  parentMap[parentName].push_back(childName);
}
void NodeMap::PopulateChildren() {
  if (!hasPopulatedChildren) {
    hasPopulatedChildren = true;
  } else {
    throw std::runtime_error("Attempted to populate children twice!");
  }
  for (const auto &pair : parentMap) {
    auto &parentName = pair.first;
    if (!HasNode(parentName)) {
      // This can happen due to lazy evaluation of parents (which allows us to do things in one pass)
      throw std::runtime_error("Parent missing from NodeMap during population: " + parentName);
    }
    auto parentNode = nodes[pair.first];
    if (parentNode == nullptr) {
      throw std::runtime_error("null ainode for parent: " + pair.first);
    }
    std::vector<aiNode *> children;
    for (const auto &childName: pair.second) {
      if (!HasNode(childName)) {
        // This should never happen, but to be safe.
        throw std::runtime_error("Child missing from NodeMap during population:" + childName);
      }
      children.emplace_back(GetNode(childName));
    }
    AssimpUtil::PopulateAiNodeChildren(parentNode, children);
  }

}

}