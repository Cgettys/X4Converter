#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Lod.h"
#include "CollisionLod.h"
#include "VisualLod.h"

namespace model {
    class Part : public AbstractElement {
    public:
        explicit Part(std::shared_ptr<ConversionContext> ctx);

        explicit Part(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) override;

        void ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) override;

        void ConvertToXml(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) override;

    private:
        bool hasRef;
        std::shared_ptr<CollisionLod> collisionLod;
        std::map<int, VisualLod> lods;
    };
}

