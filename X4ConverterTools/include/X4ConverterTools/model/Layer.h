#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include <vector>
#include "Light.h"

namespace model {
    class Layer : public AbstractElement {
    public:
        explicit Layer(std::shared_ptr<ConversionContext> ctx);

        explicit Layer(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx, int id = 0);

        explicit Layer(aiNode *node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToGameFormat(pugi::xml_node out) override;

    protected:
        int layerId;
        std::vector<Light> lights;

        void handleAiLights(aiNode *pNode);
    };

}
