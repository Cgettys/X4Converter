#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
    class Layer : public AbstractElement {
    public:
        explicit Layer(std::shared_ptr<ConversionContext> ctx);

        explicit Layer(pugi::xml_node node, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToGameFormat(pugi::xml_node out) override;
    };

}
