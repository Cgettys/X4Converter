#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include "Lod.h"

namespace model {

    const static int COLLISION_INDEX = -1;

    class CollisionLod : public Lod {
    public:
        CollisionLod() = default;

        ~CollisionLod() override = default;

        explicit CollisionLod(std::string partName, const ConversionContext &ctx);

        void ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) override;

        void ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) override;

        aiNode *ConvertToAiNode(const ConversionContext &ctx) override;

    };

}


