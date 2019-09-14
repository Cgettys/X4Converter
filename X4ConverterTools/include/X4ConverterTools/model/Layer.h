#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include <vector>
#include "Light.h"

namespace model {
    class Layer : public AbstractElement {
    public:
        explicit Layer(ConversionContext::Ptr ctx);

        explicit Layer(pugi::xml_node node, ConversionContext::Ptr ctx, int id = 0);

        explicit Layer(aiNode *node, ConversionContext::Ptr ctx);

        aiNode *ConvertToAiNode(pugi::xml_node intermediateXml) override;

        void ConvertFromAiNode(aiNode *node, pugi::xml_node intermediateXml) override;

        void ConvertToGameFormat(pugi::xml_node out) override;

    protected:
        int layerId;
        std::vector<Light> lights;

        void handleAiLights(aiNode *pNode);
    };

}
