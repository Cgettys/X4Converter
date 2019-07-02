#include <utility>

#include <X4ConverterTools/model/AbstractElement.h>
#include <vector>
#include <cstdio>
#include <iostream>
#include <X4ConverterTools/util/FormatUtil.h>

namespace model {
    AbstractElement::AbstractElement(std::shared_ptr<ConversionContext> ctx) : ctx(std::move(ctx)) {

    }

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
            std::cerr << "warning, could not read node: " << raw << std::endl;
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

    pugi::xml_node AbstractElement::Child(pugi::xml_node parent, const std::string &elementName) {
        auto result = parent.child(elementName.c_str());
        if (!result.empty()) {
            return result;
        }
        return parent.append_child(elementName.c_str());
    }

    pugi::xml_node
    AbstractElement::ChildByAttr(pugi::xml_node parent, const std::string &elemName, const std::string &attrName,
                                 const std::string &attrVal) {
        auto result = parent.find_child_by_attribute(elemName.c_str(), attrName.c_str(), attrVal.c_str());
        if (!result.empty()) {
            return result;
        }
        result = parent.append_child(elemName.c_str());
        result.append_attribute(attrName.c_str()).set_value(attrVal.c_str());
        return result;
    }

    // TODO corresponding read functions
    aiVector3D AbstractElement::ReadAttrXYZ(pugi::xml_node target) {
        return aiVector3D(target.attribute("x").as_float(0.0), target.attribute("y").as_float(0.0),
                          target.attribute("z").as_float(0.0));
    }

    // TODO make these static and use static import/ refactor into a util class
    void AbstractElement::WriteAttrXYZ(pugi::xml_node target, aiVector3D val) {
        WriteAttr(target, "x", val.x);
        WriteAttr(target, "y", val.y);
        WriteAttr(target, "z", val.z);
    }

    aiQuaternion AbstractElement::ReadAttrQuat(pugi::xml_node target) {
        return {target.attribute("qw").as_float(), target.attribute("qx").as_float(), target.attribute("qy").as_float(),
                target.attribute("qz").as_float()};
    }

    void AbstractElement::WriteAttrQuat(pugi::xml_node target, aiQuaternion val) {
        // NB: weird XML ordering for consistency with game files
        WriteAttr(target, "qx", val.x);
        WriteAttr(target, "qy", val.y);
        WriteAttr(target, "qz", val.z);
        WriteAttr(target, "qw", val.w);
    }

    void AbstractElement::WriteAttr(pugi::xml_node target, std::string name, float val) {
        std::string strVal = util::FormatUtil::formatFloat(val);
        WriteAttr(target, name, strVal);
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

    void AbstractElement::WriteOffset(pugi::xml_node target) {
        bool offsetPosZero = offsetPos.Equal(aiVector3D());
        bool offsetRotZero = offsetRot.Equal(aiQuaternion());
        auto offsetNode = Child(target, "offset");
        if (!offsetPosZero) {
            auto posNode = Child(offsetNode, "position");
            WriteAttrXYZ(posNode, offsetPos);
        } else {
            offsetNode.remove_child("position");
        }
        // TODO util method to check if zero
        if (!offsetRotZero) {
            auto quatNode = Child(offsetNode, "quaternion");
            WriteAttrQuat(quatNode, offsetRot);
        } else {
            offsetNode.remove_child("quaternion");
        }
        if (offsetPosZero && offsetRotZero) {
            target.remove_child("offset");
        }

    }

    void AbstractElement::ApplyOffsetToAiNode(aiNode *target) {
        aiMatrix4x4 tmp(aiVector3D(1, 1, 1), offsetRot, offsetPos);
        // TODO fixme upstream... this sucks
        target->mTransformation.a1 = tmp.a1;
        target->mTransformation.a2 = tmp.a2;
        target->mTransformation.a3 = tmp.a3;
        target->mTransformation.a4 = tmp.a4;
        target->mTransformation.b1 = tmp.b1;
        target->mTransformation.b2 = tmp.b2;
        target->mTransformation.b3 = tmp.b3;
        target->mTransformation.b4 = tmp.b4;
        target->mTransformation.c1 = tmp.c1;
        target->mTransformation.c2 = tmp.c2;
        target->mTransformation.c3 = tmp.c3;
        target->mTransformation.c4 = tmp.c4;
        target->mTransformation.d1 = tmp.d1;
        target->mTransformation.d2 = tmp.d2;
        target->mTransformation.d3 = tmp.d3;
        target->mTransformation.d4 = tmp.d4;

    }

    void AbstractElement::ReadOffset(pugi::xml_node target) {
        offsetRot = aiQuaternion(0, 0, 0, 0);
        offsetPos = aiVector3D(0, 0, 0);
        // A word to the wise: the XML tends to be listed qx, qy, qz, qw. Why, I do not know.
        // However, most sensible software expects qw, qx, qy, qz
        auto offsetNode = target.child("offset");
        if (offsetNode) {
            auto positionNode = offsetNode.child("position");
            if (positionNode) {
                offsetPos = ReadAttrXYZ(positionNode);
            }
            auto quaternionNode = offsetNode.child("quaternion");
            if (quaternionNode) {
                offsetRot = ReadAttrQuat(quaternionNode);
            }
            // TODO check for weird other cases
        }

    }

    aiColor3D AbstractElement::ReadAttrRGB(pugi::xml_node target) {
        return aiColor3D();
    }

    void AbstractElement::WriteAttrRGB(pugi::xml_node target, aiColor3D val) {

    }


}