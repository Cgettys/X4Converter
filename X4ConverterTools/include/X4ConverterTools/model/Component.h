#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {

    class Component : AbstractElement {
    public:
        Component(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);
    };

}
