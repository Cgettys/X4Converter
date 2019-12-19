#include <X4ConverterTools/model/MaterialCollection.h>

using namespace boost;
namespace model {
MaterialCollection::MaterialCollection() {
  _name = "";
}

MaterialCollection::MaterialCollection(pugi::xml_node &node) {
  _name = node.attribute("name").value();
  for (auto matNode : node.children("material")) {
    auto mat = Material(_name, matNode);
    _materials[mat.GetName()] = mat;
  }
}

Material *MaterialCollection::GetMaterial(const std::string &name) {
  auto it = _materials.find(name);
  if (it != _materials.end()) {
    return &(it->second);
  }
  throw std::runtime_error("Could not find material by name: " + name + " from collection: " + _name);
}
}