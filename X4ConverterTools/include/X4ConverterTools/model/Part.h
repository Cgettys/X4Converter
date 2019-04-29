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

        explicit Part(pugi::xml_node node);

        explicit Part(aiNode *node) {
            ConvertFromAiNode(node);
        }
        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

    private:
        bool hasRef;
        CollisionLod collisionLod;
        std::map<int, VisualLod> lods;
    };
}

