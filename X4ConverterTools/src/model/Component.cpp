
#include "X4ConverterTools/model/Component.h"

namespace model {
    Component::Component(pugi::xml_node node) {
        auto componentsNode = node.child("components");
        if (componentsNode.empty()){
            throw new std::runtime_error("<components> node not found");
        }

        auto componentNode = componentsNode.child("component");
        if(componentNode.empty()){
            throw new std::runtime_error("<component> node not found");
        }
        if(componentNode.next_sibling()){
            std::cerr << "Warning, this file contains more than one component. Ignoring all but the first." << std::endl;
        }

        if (!componentNode.attribute("name")) {
            throw new std::runtime_error("Unnamed component!");
        }
        for (auto attr: componentNode.attributes()) {
            auto name = std::string(attr.name());
            auto value = std::string(attr.value());
            if (name == "name") {
                setName(value);
            } else {
                attrs[name] = value;
            }
        }

    }

    aiNode *Component::ConvertToAiNode() {
        return nullptr;
    }

    void Component::ConvertFromAiNode(aiNode *node) {

    }
}