#pragma once

#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"
#include "Lod.h"
#include <map>
#include <string>

namespace model {
    class Part : public AbstractElement {
    public:
        Part() = default;

        Part(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);

    protected:
        std::map<std::string, Lod> lods;
    };
}

