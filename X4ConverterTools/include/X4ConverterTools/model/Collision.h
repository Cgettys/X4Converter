#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include "Lod.h"

namespace model {

    const static int COLLISION_INDEX = -1;
    class Collision : public Lod {
    public:
        explicit Collision(std::string partName);

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

    };

}


