#include "X4ConverterTools/model/Component.h"

#include <iostream>
namespace model {
    Component::Component(pugi::xml_node node) {
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
        for (auto attr: componentNode.attributes()) {
            auto name = std::string(attr.name());
            auto value = std::string(attr.value());
            if (name == "name") {
                setName(value);
            } else {
                attrs[name] = value;
            }
        }

        auto connectionsNode =componentNode.child("connections");
        if (connectionsNode.empty()) {
            throw std::runtime_error("No connections found!");
        }
        for (auto connectionNode : connectionsNode.children()) {
            connections.emplace_back(connectionNode,name);
        }

    }

    aiNode *Component::ConvertToAiNode() {
        auto result = new aiNode(name);
        std::map<std::string, aiNode *> nodes;
        nodes[name] = result;

        // Convert all the nodes
        for (auto conn : connections) {
            std::string connName = conn.getName();
            if (nodes.count(connName)) {
                throw std::runtime_error("Duplicate key is not allowed!" + connName);
            }
            nodes[connName] = conn.ConvertToAiNode();

            // TODO get rid of this getParts somehow
            for (auto part : conn.getParts()){
                std::string partName = part.getName();
                if (nodes.count(partName)) {
                    throw std::runtime_error("Duplicate key is not allowed!" + partName);
                }

                nodes[partName] = part.ConvertToAiNode();
            }
        }

        // Now to unflatten everything
        for (auto conn: connections) {
            auto parentName = conn.getParentName();
            if (!nodes.count(parentName)) {
                throw std::runtime_error("Missing parent \""+parentName +"\" on: \"" +conn.getName() +"\"");
            } else {
                std::cout << conn.getName() <<  " "<<parentName<<std::endl;
                auto connNode = nodes[conn.getName()];
                auto parentNode = nodes[parentName];
                parentNode->addChildren(1,&connNode);

            }
        }
        // Now double check that we didn't miss anything
        for (auto conn: connections) {
            auto connNode = nodes[conn.getName()];
            if(connNode->mParent==nullptr){
                throw std::runtime_error("UGH");
            }
        }

        auto fakeRoot = new aiNode("ROOT");
        fakeRoot->addChildren(1,&result);
        return fakeRoot;
    }

    void Component::ConvertFromAiNode(aiNode *node) {
        setName(node->mName.C_Str());
        // TODO
    }

    void Component::ConvertToXml(pugi::xml_node out) {
        // TODO asset.xmf?
    }

    unsigned long Component::getNumberOfConnections() {
        return connections.size();
    }
}