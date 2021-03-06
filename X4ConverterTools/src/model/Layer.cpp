#include "X4ConverterTools/model/Layer.h"
// TODO waypoints, trailemitters
#include <boost/format.hpp>

using namespace boost;
namespace model {
    Layer::Layer(std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {

    }

    Layer::Layer(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx, int id) : AbstractElement(ctx) {
        layerId = id;
        setName(str(format("layer%d") % layerId));
        auto lightsNode = node.child("lights");
        if (!lightsNode.empty()) {
            for (auto lightNode: lightsNode.children()) {
                lights.emplace_back(lightNode, ctx, getName());
            }
        }
    }

    model::Layer::Layer(aiNode *node, std::shared_ptr<ConversionContext> ctx) : AbstractElement(ctx) {
        ConvertFromAiNode(node);
    }

    aiNode *Layer::ConvertToAiNode() {
        auto result = new aiNode();
        result->mName = getName();

        auto lightResult = new aiNode();
        lightResult->mName = getName() + "-lights";
        // TODO should really add a Lights object or something
        std::vector<aiNode *> lightChildren;
        for (auto light: lights) {
            lightChildren.push_back(light.ConvertToAiNode());
        }
        populateAiNodeChildren(lightResult, lightChildren);

        std::vector<aiNode *> children;
        children.push_back(lightResult);
        populateAiNodeChildren(result, children);
        return result;
    }

    void Layer::ConvertFromAiNode(aiNode *node) {
        std::string name = node->mName.C_Str();
        setName(name);
        // TODO abstract out some of the shared logic with Component
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            std::string childName = child->mName.C_Str();
            if (childName == name + "-lights") {
                handleAiLights(child);
                continue;
            } else {
                throw std::runtime_error("Unexpected child name under layer: " + childName);
            }
        }


    }

    void Layer::handleAiLights(aiNode *node) {
        for (int i = 0; i < node->mNumChildren; i++) {
            auto child = node->mChildren[i];
            lights.emplace_back(child, ctx);
        }
    }

    void Layer::ConvertToGameFormat(pugi::xml_node out) {
        if (out.name() != "layer") {
            throw std::runtime_error("layer was passed incorrect node to write to!");
        }
        if (!lights.empty()) {
            auto lightsNode = AddChild(out, "lights");
            for (auto light : lights) {
                light.ConvertToGameFormat(lightsNode);
            }
        }
    }

}
