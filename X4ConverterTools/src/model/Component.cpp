#include "X4ConverterTools/model/Component.h"

#include <iostream>

namespace model {

    Component::Component(std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {}

    Component::Component(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {
        auto componentsNode = node.child("components");
        if (componentsNode.empty()) {
            throw std::runtime_error("<components> node not found");
        }

        auto componentNode = componentsNode.child("component");
        if (componentNode.empty()) {
            throw std::runtime_error("<component> node not found");
        }

        if (componentNode.next_sibling()) {
            std::cerr << "Warning, this file contains more than one component. Ignoring all but the first."
                      << std::endl;
        }

        if (!componentNode.attribute("name")) {
            throw std::runtime_error("Unnamed component!");
        }
        if (!componentNode.child("source")) {
            std::cerr << "source directory not specified" << std::endl;
        } else {
            // TODO verify not null
            std::string ref = componentNode.child("source").attribute("geometry").value();
            attrs["source"] = ref;
            ctx->SetSourcePathSuffix(ref);
        }

        for (auto attr: componentNode.attributes()) {
            auto name = std::string(attr.name());
            auto value = std::string(attr.value());
            if (name == "name") {
                setName(value);
            } else {
                attrs[name] = value;
            }
        }

        auto connectionsNode = componentNode.child("connections");
        if (connectionsNode.empty()) {
            throw std::runtime_error("No connections found!");
        }
        for (auto connectionNode : connectionsNode.children()) {
            connections.emplace_back(connectionNode, ctx, getName());
        }

        auto layersNode = componentNode.child("layers");
        if (layersNode.empty()) {
            std::cerr << "Warning, <layers> node not found";
        }
        auto layerNode = layersNode.child("layer");
        if (layerNode.next_sibling()) {
            std::cerr << "Warning, this file contains more than one layer. Ignoring all but the first." << std::endl;
        }

    }

    aiNode *Component::ConvertToAiNode() {
        auto result = new aiNode(getName());
        std::map<std::string, aiNode *> nodes;
        nodes[getName()] = result;
        populateAiNodeChildren(result, attrToAiNode());

        // Convert all the nodes
        for (auto conn : connections) {
            std::string connName = conn.getName();
            if (nodes.count(connName)) {
                throw std::runtime_error("Duplicate key is not allowed!" + connName);
            }
            nodes[connName] = conn.ConvertToAiNode();

            // TODO get rid of this getParts somehow
            for (auto part : conn.getParts()) {
                std::string partName = part.getName();
                if (nodes.count(partName)) {
                    throw std::runtime_error("Duplicate key is not allowed!" + partName);
                }
                auto partNode = nodes[connName]->FindNode(partName.c_str());
                if (partNode == nullptr) {
                    throw std::runtime_error("something has gone horribly wrong");
                }
                nodes[partName] = partNode;
            }
        }

        // Now to unflatten everything
        std::map<std::string, std::vector<aiNode *>> parentMap;
        for (auto conn: connections) {
            auto parentName = conn.getParentName();
            if (!nodes.count(parentName)) {
                throw std::runtime_error("Missing parent \"" + parentName + "\" on: \"" + conn.getName() + "\"");
            } else {
                std::cout << conn.getName() << " " << parentName << std::endl;
                auto connNode = nodes[conn.getName()];
                if (parentMap.count(parentName) == 0) {
                    parentMap[parentName] = std::vector<aiNode *>();
                }
                if (connNode == nullptr) {
                    throw std::runtime_error("null ainode");
                }
                parentMap[parentName].push_back(connNode);
            }
        }
        for (const auto &pair : parentMap) {
            auto parentNode = nodes[pair.first];
            populateAiNodeChildren(parentNode, pair.second);
        }

        // Now double check that we didn't miss anything
        for (auto conn: connections) {
            auto connNode = nodes[conn.getName()];
            if (connNode->mParent == nullptr) {
                throw std::runtime_error("connection" + conn.getName() + "lost its parent");
            }
        }
        for (const auto &pair : nodes) {
            if (pair.first == getName()) {
                continue;
            }
            if (pair.second->mParent == nullptr) {
                throw std::runtime_error(std::string(pair.second->mName.C_Str()) + "lost its parent");
            }
            for (int i = 0; i < pair.second->mNumChildren; i++) {
                if (pair.second->mChildren[i] == nullptr) {
                    throw std::runtime_error(std::string(pair.second->mName.C_Str()) + "has null child");
                }
            }
        }
        auto rootChildren = new aiNode *[1];
        rootChildren[0] = result;
        auto fakeRoot = new aiNode("ROOT");
        fakeRoot->addChildren(1, rootChildren);
        delete[] rootChildren;
        return fakeRoot;
    }

    void Component::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            std::string childName = child->mName.C_Str();
            if (childName.find("class") != std::string::npos || childName.find("source") != std::string::npos) {
                readAiNodeChild(child);
                continue;
            } else if (childName.find('*') == std::string::npos) {
                std::cerr << "Warning, possible non-component directly under root, ignoring: " << childName
                          << std::endl;
            } else {
                connections.emplace_back(child, ctx);
                recurseOnChildren(child, ctx);
            }
        }
    }

    void Component::recurseOnChildren(aiNode *tgt, std::shared_ptr<ConversionContext> ctx) {
        std::string tgtName = tgt->mName.C_Str();
        bool is_connection = tgtName.find('*') != std::string::npos;
        for (int i = 0; i < tgt->mNumChildren; i++) {
            auto child = tgt->mChildren[i];
            std::string childName = child->mName.C_Str();
            if (childName.find('*') != std::string::npos) {
                if (is_connection) {
                    throw std::runtime_error("connection cannot have a connection as a parent!");
                }
                connections.emplace_back(child, ctx, tgtName);
            }
            recurseOnChildren(child, ctx);
        }

    }

    void Component::ConvertToGameFormat(pugi::xml_node out) {
        // TODO asset.xmf?
        if (std::string(out.name()) != "components") {
            throw std::runtime_error("Component should be under components element");
        }
        auto compNode = ChildByAttr(out, "component", "name", getName());
        auto connsNode = Child(compNode, "connections");
        for (const auto &attr : attrs) {
            if (attr.first == "source") {
                ChildByAttr(compNode, "source", "geometry", attr.second);
                // TODO compare to output path and confirm if wrong
                ctx->SetSourcePathSuffix(attr.second);
            } else {
                WriteAttr(compNode, attr.first, attr.second);
            }
        }
        for (auto conn : connections) {
            conn.ConvertToGameFormat(connsNode);
        }
    }

    unsigned long Component::getNumberOfConnections() {
        return connections.size();
    }
}