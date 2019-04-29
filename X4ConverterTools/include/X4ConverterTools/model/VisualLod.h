#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

    class VisualLod : public Lod {
    public:
        VisualLod() = default;
        VisualLod(pugi::xml_node node, std::string partName);

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;
    };

}


