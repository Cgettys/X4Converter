#pragma once

#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"
#include "Connection.h"

namespace model {

    class Component : public AbstractElement {
    public:
        explicit Component(pugi::xml_node node);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

        unsigned long getNumberOfConnections();
    protected:
        std::vector<Connection> connections;
    };

}
