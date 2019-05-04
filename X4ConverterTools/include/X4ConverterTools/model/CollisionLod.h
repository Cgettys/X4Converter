#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include "Lod.h"

namespace model {

    const static int COLLISION_INDEX = -1;

    class CollisionLod : public Lod {
    public:
        explicit CollisionLod(std::shared_ptr<ConversionContext> ctx);

        explicit CollisionLod(std::string partName, std::shared_ptr<ConversionContext> ctx);

        ~CollisionLod() override = default;

        void ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) override;

        void ConvertToXml(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) override;

        aiNode *ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) override;

    };

}


