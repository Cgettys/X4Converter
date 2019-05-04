#include "X4ConverterTools/model/Part.h"

#include <string>
#include <iostream>
#include <X4ConverterTools/model/CollisionLod.h>
#include <X4ConverterTools/model/VisualLod.h>
#include <regex>

namespace model {
    Part::Part(pugi::xml_node node, const ConversionContext &ctx) {
        if (std::string(node.name()) != "part") {
            throw std::runtime_error("XML element must be a <part> element!");
        }
        if (node.attribute("name").empty()) {
            throw std::runtime_error("Part must have a name attribute!");
        }
        hasRef = false;
        for (auto attr: node.attributes()) {
            auto attrName = std::string(attr.name());
            if (attrName == "ref") {
                hasRef = true;
                attrs["DO_NOT_EDIT.ref"] = attr.value();
            } else if (attrName == "name") {
                setName(attr.value());
            } else {
                std::cerr << "Warning, unhandled attribute on part: " << getName() << " attribute: " << attrName
                          << ". This may work fine, just a heads up ;)" << std::endl;
                attrs[attrName] = attr.value();
            }
        }

        auto lodsNode = node.child("lods");
        if (hasRef && !lodsNode.empty()) {
            throw std::runtime_error("ref should not contain lods");
        }

        if (!hasRef) {
            collisionLod = CollisionLod(getName(), ctx);

            for (auto lodNode : lodsNode.children()) {
                auto lod = VisualLod(lodNode, getName(), ctx);
                lods.insert(std::pair<int, VisualLod>(lod.getIndex(), lod));
            }
        }

    }

    Part::Part(aiNode *node, const ConversionContext &ctx) {
        ConvertFromAiNode(node, ctx);
    }

    aiNode *Part::ConvertToAiNode(const ConversionContext &ctx) {
        auto *result = new aiNode(getName());
        std::vector<aiNode *> children = attrToAiNode();
        if (!hasRef) {
            children.push_back(collisionLod.ConvertToAiNode(ctx));
            for (auto lod: lods) {
                children.push_back(lod.second.ConvertToAiNode(ctx));
            }
        }

        populateAiNodeChildren(result, children);


        return result;
    }

    static std::regex lodRegex("[^|]+\\|lod\\d");
    static std::regex collisionRegex("[^|]+\\|collision");

    void Part::ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) {
        std::string tmp = std::string();
        setName(node->mName.C_Str());
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            std::string childName = child->mName.C_Str();
            // TODO check part names?
            if (regex_match(childName, lodRegex)) {
                auto lod = VisualLod();
                lod.ConvertFromAiNode(child, ctx);
                lods.insert(std::pair<int, VisualLod>(lod.getIndex(), lod));
            } else if (regex_match(childName, collisionRegex)) {
                collisionLod = CollisionLod();
                collisionLod.ConvertFromAiNode(child, ctx);
            } else if (childName.find('*') != std::string::npos) {
                // Ignore connection, handled elsewhere
            } else {
                readAiNodeChild(child);
            }
        }
        // TODO more
    }

    void Part::ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) {
        if (std::string(out.name()) != "parts") {
            throw std::runtime_error("part must be appended to a parts xml element");
        }

        auto partNode = ChildByAttr(out, "part", "name", getName());


        // Note the return statement! referenced parts don't get LODS!!!
        // TODO remove if lods exist or at least error out
        if (attrs.count("DO_NOT_EDIT.ref")) {
            hasRef = true;
            auto value = attrs["DO_NOT_EDIT.ref"];
            if (partNode.attribute("ref")) {
                partNode.attribute("ref").set_value(value.c_str());
            } else {
                partNode.prepend_attribute("ref").set_value(value.c_str());
            }
            return;
        }
        for (auto attr : attrs) {
            WriteAttr(partNode, attr.first, attr.second);
        }

        if (!lods.empty()) {
            auto lodsNode = Child(partNode, "lods");
            collisionLod.ConvertToXml(lodsNode, ctx); // TODO
            for (auto lod : lods) {
                lod.second.ConvertToXml(lodsNode, ctx);
            }
        } else {
            partNode.remove_child("lods");
        }
        // TODO out more
    }
}