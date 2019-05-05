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

        aiNode *ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) override;

        void ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) override;

        void ConvertToGameFormat(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) override;

        unsigned long getNumberOfConnections();

    protected:
        std::vector<Connection> connections;

        void recurseOnChildren(aiNode *child, std::shared_ptr<ConversionContext> ctx);
    };

}
