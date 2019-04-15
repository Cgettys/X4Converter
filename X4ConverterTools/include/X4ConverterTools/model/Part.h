#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
    class Part : public AbstractElement {
    public:
        Part() = default;
        Part(pugi::xml_node node);

        virtual aiNode *ConvertToAiNode();
    };
}

