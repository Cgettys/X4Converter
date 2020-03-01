#include "X4ConverterTools/util/MetadataStore.h"

#include <utility>
namespace util {
MetadataStore::MetadataStore(std::string metadataFilePath) : metadataPath(std::move(metadataFilePath)) {

}

void MetadataStore::SetAll(std::map<std::string, MetadataMap> in) {
  allMetadata = std::move(in);
}
std::map<std::string, MetadataStore::MetadataMap> MetadataStore::GetAll() {
  return allMetadata;
}
void MetadataStore::WriteDocument() {
  pugi::xml_document metadoc;
  auto metaroot = metadoc.root();
  WriteXml(metaroot);
  metadoc.save_file(metadataPath.c_str());
}
void MetadataStore::WriteXml(pugi::xml_node &target) {
  auto kvNode = target.child("misc");
  for (const auto &obj : allMetadata) {
    auto objNode = kvNode.child("object");
    objNode.child("name").set_value(obj.first.c_str());
    for (const auto &pair : obj.second) {
      auto attrNode = objNode.child("attribute");
      attrNode.child("key").set_value(pair.first.c_str());
      attrNode.child("value").set_value(pair.second.c_str());
    }
  }
}

bool MetadataStore::HasAttribute(const std::string &name, const std::string &key) {
  if (allMetadata.count(name) > 0) {
    if (allMetadata.count(key) > 0) {
      return true;
    }
  }
  return false;
}

std::string MetadataStore::GetAttribute(const std::string &name, const std::string &key) {
  if (!HasAttribute(name, key)) {
    throw std::runtime_error("Attribute not found!");
  }
  return allMetadata[name][key];
}

void MetadataStore::SetAttribute(const std::string &name, const std::string &key, std::string value) {
  if (allMetadata.count(name) <= 0) {
    allMetadata[name] = {};
  }
  allMetadata[name][key] = std::move(value);
}
MetadataStore::MetadataMap &MetadataStore::GetMetadata(const std::string &name) {
  if (allMetadata.count(name) <= 0) {
    allMetadata[name] = {};
  }
  return allMetadata[name];
}
}