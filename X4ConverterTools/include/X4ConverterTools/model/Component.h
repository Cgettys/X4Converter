#pragma once

#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"
#include "Connection.h"

namespace model {

    class Component : public AbstractElement {
    public:
        explicit Component(std::shared_ptr<ConversionContext> ctx);

        explicit Component(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToGameFormat(pugi::xml_node out) override;

        unsigned long getNumberOfConnections();

    protected:
        std::vector<Connection> connections;

        void recurseOnChildren(aiNode *child, std::shared_ptr<ConversionContext> ctx);
    };

}
