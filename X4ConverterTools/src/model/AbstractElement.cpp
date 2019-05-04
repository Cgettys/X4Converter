#include <X4ConverterTools/model/AbstractElement.h>
#include <vector>
#include <cstdio>
#include <iostream>

namespace model {

    std::string AbstractElement::getName() {
        return name;
    }

    void AbstractElement::setName(std::string n) {
        name = std::move(n);
    }


    std::vector<aiNode *> AbstractElement::attrToAiNode() {
        std::vector<aiNode *> children;
        for (const auto &attr : attrs) {
            children.push_back(GenerateAttrNode(attr.first, attr.second));
        }
        return children;
    }

    void AbstractElement::populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children) {
        unsigned long numChildren = children.size();
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


    void AbstractElement::readAiNodeChild(aiNode *source) {
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

    pugi::xml_node AbstractElement::Child(pugi::xml_node parent, std::string elementName) {
        auto result = parent.child(elementName.c_str());
        if (!result.empty()) {
            return result;
        }
        return parent.append_child(elementName.c_str());
    }

    pugi::xml_node AbstractElement::ChildByAttr(pugi::xml_node parent, std::string elemName, std::string attrName,
                                                std::string attrVal) {
        auto result = parent.find_child_by_attribute(elemName.c_str(), attrName.c_str(), attrVal.c_str());
        if (!result.empty()) {
            return result;
        }
        result = parent.append_child(elemName.c_str());
        result.append_attribute(attrName.c_str()).set_value(attrVal.c_str());
        return result;
    }


    void AbstractElement::WriteAttr(pugi::xml_node target, std::string name, std::string val) {
        auto attr = target.attribute(name.c_str());
        if (attr.empty()) {
            attr = target.append_attribute(name.c_str());
        }
        attr.set_value(val.c_str());
    }

    aiNode *AbstractElement::GenerateAttrNode(const std::string &key, const std::string &value) {
        return new aiNode(name + "|" + key + "|" + value);
    }

}