#include "X4ConverterTools/util/MetadataStore.h"

#include <utility>
#include <X4ConverterTools/util/XmlUtil.h>
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
  auto kvNode = XmlUtil::AddChild(target, "misc");
  for (const auto &obj : allMetadata) {
    auto objNode = XmlUtil::AddChildByAttr(kvNode, "object", "name", obj.first.c_str());
    for (const auto &pair : obj.second) {
      auto attrNode = XmlUtil::AddChild(objNode, "attribute");
      XmlUtil::AddChild(attrNode, "key").set_value(pair.first.c_str());
      XmlUtil::AddChild(attrNode, "value").set_value(pair.second.c_str());
    }
  }
}

bool MetadataStore::HasAttribute(const std::string &name, const std::string &key) {
  return allMetadata.count(name) > 0 && allMetadata[name].count(key) > 0;
}

std::string MetadataStore::GetAttribute(const std::string &name, const std::string &key) {
  if (!HasAttribute(name, key)) {
    throw std::runtime_error("Attribute " + key + " not found for name: " + name + "!");
  }
  return allMetadata[name][key];
}

void MetadataStore::SetAttribute(const std::string &name, const std::string &key, const std::string &value) {
  if (allMetadata.count(name) <= 0) {
    allMetadata.insert({name, {}});
  }
  // Note: insert wouldn't overwrite here
  allMetadata[name].insert_or_assign(key, value);
}
MetadataStore::MetadataMap &MetadataStore::GetMetadata(const std::string &name) {
  if (allMetadata.count(name) <= 0) {
    allMetadata[name] = {};
  }
  return allMetadata[name];
}
}