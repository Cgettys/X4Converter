#pragma once

#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"
#include "Connection.h"

namespace model {

    class Component : public AbstractElement {
    public:
        Component() = default;

        explicit Component(pugi::xml_node node, const ConversionContext &ctx);

        aiNode *ConvertToAiNode(const ConversionContext &ctx) override;

        void ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) override;

        void ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) override;

        unsigned long getNumberOfConnections();

    protected:
        std::vector<Connection> connections;

        void recurseOnChildren(aiNode *child, const ConversionContext &ctx);
    };

}
