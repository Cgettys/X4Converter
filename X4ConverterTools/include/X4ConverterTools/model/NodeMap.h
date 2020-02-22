#pragma once

#include <X4ConverterTools/util/AssimpUtil.h>
namespace model {
// Class to ease handling of unflattening the document structure
class NodeMap {
 public:
  // Creates a new aiNode, adds it to the map, and returns the new node to the caller
  aiNode *CreateNode(const std::string &name);
  void AddNode(aiNode *node);
  bool HasNode(const std::string &name);
  aiNode *GetNode(const std::string &name);
  void Print();
  void MakeParent(const std::string &parentName, const std::string &childName);
  void PopulateChildren();
 private:
  bool hasPopulatedChildren = false;
  std::map<std::string, aiNode *> nodes;
  std::map<std::string, std::vector<std::string>> parentMap;
  void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children);
};
}