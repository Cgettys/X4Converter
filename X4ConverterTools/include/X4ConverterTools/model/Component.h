#pragma once

#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"

namespace model {

    class Component : public AbstractElement {
    public:
        Component(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);
    };

}
