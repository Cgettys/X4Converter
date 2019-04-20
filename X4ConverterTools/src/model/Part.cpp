#include "X4ConverterTools/model/Part.h"

namespace model {
    Part::Part(pugi::xml_node node) {
        if (std::string(node.name()) != "part") {
            throw std::runtime_error("XML element must be a <part> element!");
        }
        if (node.attribute("name").empty()) {
            throw std::runtime_error("Part must have a name attribute!");
        }
        for (auto attr: node.attributes()) {
            auto attrName = std::string(attr.name());
            if (attrName == "ref") {
                isRef = true;
                ref = attr.value();
            } else if (attrName == "name") {
                setName(attr.value());
            } else {
                std::cerr << "Warning, unhandled attribute on part: " << name << " attribute: " << attrName
                          << std::endl;
            }
        }
    }

    aiNode *Part::ConvertToAiNode() {
        auto result = new aiNode(name);
        if (isRef) {
            result->mNumChildren = 1;
            result->mChildren = new aiNode *[1];
            auto child = new aiNode("DO_NOT_EDIT^ref^" + ref);
            child->mParent = result;
            result->mChildren[0] = child;
        }

        return result;
    }

    void Part::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
    }
}