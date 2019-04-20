#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
namespace model {
    Part::Part(pugi::xml_node node) {
        if (std::string(node.name()) != "part") {
            throw std::runtime_error("XML element must be a <part> element!");
        }
        if (node.attribute("name").empty()) {
            throw std::runtime_error("Part must have a name attribute!");
        }
        bool hasRef = false;
        for (auto attr: node.attributes()) {
            auto attrName = std::string(attr.name());
            if (attrName == "ref") {
                hasRef = true;
                attrs["DO_NOT_EDIT.ref"] = attr.value();
            } else if (attrName == "name") {
                setName(attr.value());
            } else {
                std::cerr << "Warning, unhandled attribute on part: " << name << " attribute: " << attrName
                          << std::endl;
            }
        }

        auto lodsNode = node.child("lods");
        if (hasRef && !lodsNode.empty()) {
            throw std::runtime_error("ref should not contain lods");
        }

        if (!hasRef) {
            auto collisionLod = new CollisionLod(name);
            lods.insert(std::pair<int, CollisionLod *>(collisionLod->getIndex(), collisionLod));
            for (auto lodNode : lodsNode.children()) {
                auto lod = new VisualLod(lodNode, name);
                lods.insert(std::pair<int, VisualLod *>(lod->getIndex(), lod));
            }
        }

    }

    aiNode *Part::ConvertToAiNode() {
        auto *result = new aiNode(name);
        std::vector<aiNode *> children;
        for (auto lod: lods) {
            children.push_back(lod.second->ConvertToAiNode());
        }
        populateAiNodeChildren(result, children);

        return result;
    }

    void Part::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
    }

    void Part::ConvertToXml(pugi::xml_node out) {

    }
}