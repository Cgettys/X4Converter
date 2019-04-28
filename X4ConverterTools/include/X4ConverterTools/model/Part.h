#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"
#include "Lod.h"

namespace model {
    class Part : public AbstractElement {
    public:
        Part() = default;

        explicit Part(pugi::xml_node node);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

    private:
        std::map<int, Lod *> lods;
    };
}
