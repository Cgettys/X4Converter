#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include "Lod.h"

namespace model {

    const static int COLLISION_INDEX = -1;

    class CollisionLod : public Lod {
    public:
        CollisionLod() = default;
        explicit CollisionLod(std::string partName);

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

    };

}


