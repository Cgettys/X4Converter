#pragma once

#include <map>
#include <boost/format.hpp>
#include <boost/range.hpp>
#include <X4ConverterTools/model/Material.h>
#include <pugixml.hpp>

namespace model {
class MaterialCollection {
 public:
  // TODO help with typos?
  explicit MaterialCollection(pugi::xml_node &node);

  ~MaterialCollection() = default;

  Material &GetMaterial(const std::string &name);

  [[nodiscard]] const std::string &GetName() const { return _name; }

 private:

  std::string _name;
  std::map<std::string, Material> _materials;
};
}