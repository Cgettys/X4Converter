#pragma once

#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"
#include "Lod.h"
#include <string>
#include <X4ConverterTools/model/Collision.h>

namespace model {
    class Part : public AbstractElement {
    public:
        Part() = default;

        Part(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);

        void ConvertToXml(pugi::xml_node out);

    private:
        std::map<int, Lod> lods;
    };
}

