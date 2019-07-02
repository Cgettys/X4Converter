#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Lod.h"
#include "CollisionLod.h"
#include "VisualLod.h"
#include "Light.h"

namespace model {
    class Part : public AbstractElement {
    public:
        explicit Part(std::shared_ptr<ConversionContext> ctx);

        explicit Part(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToGameFormat(pugi::xml_node out) override;

    private:
        bool hasRef;
        std::shared_ptr<CollisionLod> collisionLod;
        std::map<int, VisualLod> lods;
        std::vector<Light> lights;

        void handleAiLights(aiNode *node);
    };
}

