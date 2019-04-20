#include "X4ConverterTools/model/Component.h"

namespace model {
    Component::Component(pugi::xml_node node) {
    }

    aiNode *Component::ConvertToAiNode() {
        return nullptr;
    }

    void Component::ConvertFromAiNode(aiNode *node) {

    }
}