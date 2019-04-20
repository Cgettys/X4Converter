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
                attrs["DO_NOT_EDIT.ref"] = attr.value();
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
        populateAiNodeChildren(result,std::vector<aiNode*>());
        return result;
    }

    void Part::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
    }
}