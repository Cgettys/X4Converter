#include "X4ConverterTools/model/Connection.h"

#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <X4ConverterTools/util/FormatUtil.h>

using namespace util;
namespace model {

    Connection::Connection(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx, std::string componentName)
            : AbstractElement(ctx) {

        if (!node.attribute("name")) {
            throw std::runtime_error("Unnamed connection!");
        }
        parentName = std::move(componentName);//Default to component as parent

        offsetRot = aiQuaternion(0, 0, 0, 0);
        offsetPos = aiVector3D(0, 0, 0);
        // A word to the wise: the XML tends to be listed qx, qy, qz, qw. Why, I do not know.
        // However, most sensible software expects qw, qx, qy, qz
        auto offsetNode = node.child("offset");
        if (offsetNode) {
            auto positionNode = offsetNode.child("position");
            if (positionNode) {
                offsetPos = aiVector3D(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(),
                                       positionNode.attribute("z").as_float());
            }
            auto quaternionNode = offsetNode.child("quaternion");
            if (quaternionNode) {
                offsetRot = aiQuaternion(quaternionNode.attribute("qw").as_float(),
                                         quaternionNode.attribute("qx").as_float(),
                                         quaternionNode.attribute("qy").as_float(),
                                         quaternionNode.attribute("qz").as_float());
            }
            // TODO check for weird other cases
        }

        auto partsNode = node.child("parts");
        if (partsNode) {
            for (auto child : partsNode.children()) {
                parts.emplace_back(child, ctx);
            }
        }
        for (auto attr: node.attributes()) {
            auto attrName = std::string(attr.name());
            auto value = std::string(attr.value());
            if (attrName == "name") {
                setName(value);
            } else if (attrName == "parent") {
                parentName = value;
            } else {
                attrs[attrName] = value;
            }
        }
    }

    Connection::Connection(aiNode *node, std::shared_ptr<ConversionContext> ctx, std::string componentName)
            : AbstractElement(ctx) {
        ConvertFromAiNode(node, ctx);
        parentName = std::move(componentName);//Default to component as parent

    }

    aiNode *Connection::ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) {
        auto result = new aiNode("*" + getName() + "*");
        aiMatrix4x4 tmp(aiVector3D(1, 1, 1), offsetRot, offsetPos);
        // TODO fixme upstream... this sucks
        result->mTransformation.a1 = tmp.a1;
        result->mTransformation.a2 = tmp.a2;
        result->mTransformation.a3 = tmp.a3;
        result->mTransformation.a4 = tmp.a4;
        result->mTransformation.b1 = tmp.b1;
        result->mTransformation.b2 = tmp.b2;
        result->mTransformation.b3 = tmp.b3;
        result->mTransformation.b4 = tmp.b4;
        result->mTransformation.c1 = tmp.c1;
        result->mTransformation.c2 = tmp.c2;
        result->mTransformation.c3 = tmp.c3;
        result->mTransformation.c4 = tmp.c4;
        result->mTransformation.d1 = tmp.d1;
        result->mTransformation.d2 = tmp.d2;
        result->mTransformation.d3 = tmp.d3;
        result->mTransformation.d4 = tmp.d4;

        std::vector<aiNode *> children = attrToAiNode();
        for (auto part : parts) {
            children.push_back(part.ConvertToAiNode(ctx));
        }
        populateAiNodeChildren(result, children);
        return result;
    }

    std::string Connection::getParentName() {
        return parentName;
    }


    void Connection::ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) {
        std::string tmp = node->mName.C_Str();
        setName(tmp.substr(1, tmp.size() - 2));
        // TODO check for scaling and error if cfound
        node->mTransformation.DecomposeNoScaling(offsetRot, offsetPos);

        // TODO validate attributes; better check for parts & a better solution
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            auto childName = std::string(child->mName.C_Str());
            if (childName.find('|') != std::string::npos) {
                readAiNodeChild(child);
            } else {
                Part part(ctx);
                part.ConvertFromAiNode(child, ctx);
                parts.emplace(parts.end(), part);
            }
        }
    }

    void Connection::ConvertToXml(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) {
        if (std::string(out.name()) != "connections") {
            throw std::runtime_error("parent of connection must be connections xml element");
        }
        auto node = ChildByAttr(out, "connection", "name", getName());

        if (!parentName.empty()) {
            WriteAttr(node, "parent", parentName);
        }

        for (const auto &pair : attrs) {
            WriteAttr(node, pair.first, pair.second);
        }

        bool offsetPosZero = offsetPos.Equal(aiVector3D());
        bool offsetRotZero = offsetRot.Equal(aiQuaternion());
        auto offsetNode = Child(node, "offset");
        if (!offsetPosZero) {
            auto posNode = Child(offsetNode, "position");
            WriteAttr(posNode, "x", FormatUtil::formatFloat(offsetPos.x));
            WriteAttr(posNode, "y", FormatUtil::formatFloat(offsetPos.y));
            WriteAttr(posNode, "z", FormatUtil::formatFloat(offsetPos.z));
        } else {
            offsetNode.remove_child("position");
        }
        if (!offsetRotZero) {
            auto quatNode = Child(offsetNode, "quaternion");
            // NB: weird XML ordering
            WriteAttr(quatNode, "qx", FormatUtil::formatFloat(offsetRot.x));
            WriteAttr(quatNode, "qy", FormatUtil::formatFloat(offsetRot.y));
            WriteAttr(quatNode, "qz", FormatUtil::formatFloat(offsetRot.z));
            WriteAttr(quatNode, "qw", FormatUtil::formatFloat(offsetRot.w));
        } else {
            offsetNode.remove_child("quaternion");
        }
        if (offsetPosZero && offsetRotZero) {
            node.remove_child("offset");
        }


        if (parts.empty()) {
            return;
        }
        auto partsNode = Child(node, "parts");
        for (auto part : parts) {
            part.ConvertToXml(partsNode, ctx);
        }

    }
}