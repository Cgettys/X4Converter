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
            name = n;
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
            int numChildren = children.size() + attrs.size();
            target->mNumChildren = numChildren;
            target->mChildren = new aiNode *[numChildren];

            // TODO can we use addChildren?
            int idx = 0;
            for (auto child : children) {
                target->mChildren[idx++] = child;
            }
            for (auto attr : attrs) {
                target->mChildren[idx++] = GenerateAttrNode(target, attr.first, attr.second);
            }

        }

        virtual aiNode *GenerateAttrNode(aiNode *parent, std::string key, std::string value) {
            auto result = new aiNode(name + "^" + key + "^" + value);
            result->mParent = parent;
            return result;
        }

        std::map<std::string, std::string> attrs;
        std::string name;
    };


}