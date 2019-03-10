#include <X4ConverterTools/MaterialCollection.h>

using namespace boost;

MaterialCollection::MaterialCollection() {
    _name = "";
}


MaterialCollection::MaterialCollection(pugi::xml_node node) {
    _node = node;
    _name = node.attribute("name").value();
}

Material *MaterialCollection::GetMaterial(const std::string &name) {
    auto it = _materials.find(name);
    if (it != _materials.end()) {
        return &(it->second);
    }

    pugi::xml_node materialNode = _node.select_node(
            (format("material[@name='%s']") % name.c_str()).str().c_str()).node();
    if (!materialNode) {
        return nullptr;
    }
    _materials[name] = Material(_name, materialNode);
    return &_materials[name];
}
