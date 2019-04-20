#include <utility>
#include "X4ConverterTools/model/Connection.h"

namespace model {

    Connection::Connection(pugi::xml_node node, std::string componentName) {

        if (!node.attribute("name")) {
            throw std::runtime_error("Unnamed connection!");
        }
        parentName = std::move(componentName);//Default to component as parent

        offsetPos = aiVector3D(0, 0, 0);
        offsetRot = aiQuaternion(0, 0, 0, 0);
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
                offsetRot = aiQuaternion(quaternionNode.attribute("qw").as_float(), quaternionNode.attribute("qx").as_float(),
                                         quaternionNode.attribute("qy").as_float(), quaternionNode.attribute("qz").as_float());
            }
            // TODO check for weird other cases
        }

        auto partsNode = node.child("parts");
        if (partsNode) {
            for (auto child : partsNode.children()) {
                parts.emplace_back(child);
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
        std::cerr << name<<std::endl;
    }

    aiNode *Connection::ConvertToAiNode() {
        auto result = new aiNode(name);
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

        std::vector<aiNode *> children;
        for (auto part : parts) {
            children.push_back(part.ConvertToAiNode());
        }
        populateAiNodeChildren(result, children);
        return result;
    }

    void Connection::ConvertFromAiNode(aiNode *node) {


    }

    std::string Connection::getParentName() {
        return parentName;
    }

    void Connection::ConvertToXml(pugi::xml_node out) {

    }
}