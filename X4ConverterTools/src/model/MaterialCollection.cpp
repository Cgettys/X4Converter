#include <X4ConverterTools/model/MaterialCollection.h>

using namespace boost;
namespace model {
    MaterialCollection::MaterialCollection() {
        _name = "";
    }


    MaterialCollection::MaterialCollection(pugi::xml_node node) {
        _node = node;
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

        pugi::xml_node materialNode = _node.select_node(
                (format("material[@name='%s']") % name.c_str()).str().c_str()).node();
        if (!materialNode) {
            return nullptr;
        }
        _materials[name] = Material(_name, materialNode);
        return &_materials[name];
    }
}