#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
    enum LightKind {
        arealight, omni, box
    };

    class Light : public AbstractElement {
    public:
        explicit Light(std::shared_ptr<ConversionContext> ctx);

        explicit Light(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx, std::string parentName);

        explicit Light(aiNode *node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToGameFormat(pugi::xml_node out) override;

    private:
        LightKind lightKind;
        aiColor3D color;
        aiVector2D area;
        bool lightEffect;
        float range;
        float shadowRange;
        float radius;
        float spotAttenuation;
        float specularIntensity;
        bool trigger; // TODO confirm bools
        float intensity;
    };
}

