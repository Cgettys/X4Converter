//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <iostream>
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

        // TODO .cpp file
        // TODO template pattern?
        virtual void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children) {
            unsigned long numChildren = children.size() + attrs.size();
            for (const auto &attr : attrs) {
                children.push_back(GenerateAttrNode(attr.first, attr.second));
            }

            if (children.empty()) {
                return;
            }

            auto oldCount = target->mNumChildren;
            auto newCount = oldCount + numChildren;
            auto arr = new aiNode *[newCount];
            auto oldLen = oldCount * sizeof(aiNode *);

            if (target->mChildren != nullptr) {
                memcpy(arr, target->mChildren, oldLen);
            }
            memcpy(arr + oldCount, children.data(), numChildren * sizeof(aiNode *));
            target->mNumChildren = newCount;
            auto old = target->mChildren;
            target->mChildren = arr;
            for (int i = 0; i < newCount; i++) {
                auto child = arr[i];
                child->mParent = target;

            }
            delete[] old;
        }

        virtual aiNode *GenerateAttrNode(const std::string &key, const std::string &value) {
            return new aiNode(name + "|" + key + "|" + value);
        }

        virtual void readAiNodeChild(aiNode *source) {
            std::string raw = source->mName.C_Str();
            auto firstSplit = raw.find('|');
            auto secondSplit = raw.rfind('|');
            if (firstSplit == secondSplit || firstSplit == std::string::npos || secondSplit == std::string::npos) {
                std::cerr << "warning, could not read node" << raw << std::endl;
                return;
            }
            auto namePart = raw.substr(0, firstSplit);
            auto tagPart = raw.substr(firstSplit + 1, secondSplit - firstSplit - 1);
            auto valPart = raw.substr(secondSplit + 1);
            if (namePart != name) {
                std::cerr << "Warning, name of element was " + name + " but tag was for name: " + namePart << std::endl;
            }
            attrs[tagPart] = valPart;
        }

        virtual pugi::xml_node getOrMakeChild(pugi::xml_node parent, std::string elementName) {
            auto result = parent.child(elementName.c_str());
            if (!result.empty()) {
                return result;
            }
            return parent.append_child(elementName.c_str());
        }

        virtual pugi::xml_node
        getOrMakeChildByAttr(pugi::xml_node parent, std::string elementName, std::string attrName,
                             std::string attrValue) {
            auto result = parent.find_child_by_attribute(elementName.c_str(), attrName.c_str(), attrValue.c_str());
            if (!result.empty()) {
                return result;
            }
            result = parent.append_child(elementName.c_str());
            result.append_attribute(attrName.c_str()).set_value(attrValue.c_str());
            return result;
        }

        virtual void createOrOverwriteAttr(pugi::xml_node target, std::string name, std::string val) {
            auto attr = target.attribute(name.c_str());
            if (attr.empty()) {
                attr = target.append_attribute(name.c_str());
            }
            attr.set_value(val.c_str());
        }


        std::map<std::string, std::string> attrs;
        std::string name;
    };


}