//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include <assimp/scene.h>

namespace model {
    class AbstractElement {
    public:
        virtual void setName(std::string n) {
            name = std::move(n);
        }

        virtual std::string getName() {
            return name;
        }

        virtual aiNode *ConvertToAiNode() = 0;

        virtual void ConvertFromAiNode(aiNode *node) = 0;

        virtual void ConvertToXml(pugi::xml_node out) = 0;

    protected:

        virtual void ParseAttrNode(aiNode *node) {

        }

        // TODO template pattern?
        virtual void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children) {
            unsigned long numChildren = children.size() + attrs.size();
            for (const auto &attr : attrs) {
                children.push_back(GenerateAttrNode(attr.first, attr.second));
            }
            target->addChildren(static_cast<unsigned int>(numChildren), children.data());
        }

        virtual aiNode *GenerateAttrNode(const std::string &key, const std::string &value) {
            return new aiNode(name + "|" + key + "|" + value);

        }

        std::map<std::string, std::string> attrs;
        std::string name;
    };


}