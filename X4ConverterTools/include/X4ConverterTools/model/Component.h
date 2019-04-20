#pragma once

#include <pugixml.hpp>
#include <iostream>
#include <vector>
#include "AbstractElement.h"
#include "Connection.h"

namespace model {

    class Component : public AbstractElement {
    public:
        Component(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);
        void ConvertToXml(pugi::xml_node out);

    protected:
        std::vector<Connection> connections;
    };

}
