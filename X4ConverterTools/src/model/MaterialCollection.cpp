#include <X4ConverterTools/model/MaterialCollection.h>

using namespace boost;
namespace model {


MaterialCollection::MaterialCollection(pugi::xml_node &node) {
  _name = node.attribute("name").value();
  for (auto &matNode : node.children("material")) {
    std::string matName = matNode.attribute("name").value();
    // Idiomatic but weird way to check if element was already present
    // First argument = key, remainder forwarded to Material's constructor
    if (!_materials.try_emplace(matName, _name, matNode).second) {
      throw std::runtime_error("Duplicated material with name:" + matName);
    }
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