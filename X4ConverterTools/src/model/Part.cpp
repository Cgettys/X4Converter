#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
#include <regex>
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

    static std::regex lodRegex("[a-zA-Z]+\\|lod\\d");
    static std::regex collisionRegex("[a-zA-Z]+\\|collision");
    void Part::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            std::string childName = child->mName.C_Str();
            // TODO check part names?
            if (regex_match(childName, lodRegex)) {
                auto lod = new VisualLod();
                lod->ConvertFromAiNode(child);
                lods.insert(std::pair<int, VisualLod *>(lod->getIndex(), lod));
            } else if (regex_match(childName, collisionRegex)) {
                auto lod = new CollisionLod();
                lod->ConvertFromAiNode(child);
                lods.insert(std::pair<int, CollisionLod *>(lod->getIndex(), lod));
            } else {
                // TODO warn/error
            }
        }
        // TODO more
    }

    void Part::ConvertToXml(pugi::xml_node out) {
        if (std::string(out.name()) != "parts") {
            throw std::runtime_error("part must be appended to a parts xml element");
        }

        auto partNode = out.find_child_by_attribute("part", "name", name.c_str());
        if (partNode.empty()) {
            partNode = out.append_child("part");
            partNode.append_attribute("name").set_value(name.c_str());
        }


        if (!lods.empty()) {
            auto lodsNode = partNode.child("lods");
            if (lodsNode.empty()) {
                lodsNode = partNode.append_child("lods");
            }

            for (auto lod : lods) {
                lod.second->ConvertToXml(lodsNode);
            }
        }
        // TODO out more
    }
}