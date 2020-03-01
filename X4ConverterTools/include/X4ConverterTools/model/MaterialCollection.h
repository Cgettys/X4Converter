#pragma once

#include <map>
#include <boost/format.hpp>
#include <boost/range.hpp>
#include <X4ConverterTools/model/Material.h>
#include <X4ConverterTools/util/FileSystemUtil.h>
#include <pugixml.hpp>

namespace model {
class MaterialCollection {
 public:
  // TODO help with typos?
  explicit MaterialCollection(util::FileSystemUtil::Ptr c, pugi::xml_node &node);

  ~MaterialCollection() = default;

  Material &GetMaterial(const std::string &name);

  [[nodiscard]] const std::string &GetName() const { return _name; }

 private:
  const util::FileSystemUtil::Ptr fsUtil;
  std::string _name;
  std::map<std::string, Material> _materials;
};
}