#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Lod.h"
#include "CollisionLod.h"
#include "VisualLod.h"

namespace model {
    class Part : public AbstractElement {
    public:
        Part() = default;

        explicit Part(pugi::xml_node node, const ConversionContext &ctx);

        explicit Part(aiNode *node, const ConversionContext &ctx);

        aiNode *ConvertToAiNode(const ConversionContext &ctx) override;

        void ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) override;

        void ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) override;

    private:
        bool hasRef;
        CollisionLod collisionLod;
        std::map<int, VisualLod> lods;
    };
}

