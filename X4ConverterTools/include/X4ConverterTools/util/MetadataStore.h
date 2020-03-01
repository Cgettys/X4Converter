#pragma once
#include <map>
#include <pugixml.hpp>
#include <memory>
namespace util {
// A class to represent everything which won't go through Assimp nicely
class MetadataStore {
 public:
  using Ptr = std::unique_ptr<MetadataStore>;
  using MetadataMap = std::map<std::string, std::string>;
  explicit MetadataStore(std::string metadataFilePath);
  bool HasAttribute(const std::string &name, const std::string &key);
  std::string GetAttribute(const std::string &name, const std::string &key);
  void SetAttribute(const std::string &name, const std::string &key, std::string value);
  MetadataMap &GetMetadata(const std::string &name);
//TODO remove these two when read/write works
  std::map<std::string, MetadataMap> GetAll();
  void SetAll(std::map<std::string, MetadataMap> in);
  void WriteXml(pugi::xml_node &target);
  void WriteDocument();
 private:
  const std::string metadataPath;
  std::map<std::string, MetadataMap> allMetadata;
};
}