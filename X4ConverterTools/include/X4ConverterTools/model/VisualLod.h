#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

    class VisualLod : public Lod {
    public:
        explicit VisualLod(std::shared_ptr<ConversionContext> ctx);

        VisualLod(pugi::xml_node node, std::string partName, std::shared_ptr<ConversionContext> ctx);

        aiNode *ConvertToAiNode(std::shared_ptr<ConversionContext> ctx) override;

        void ConvertFromAiNode(aiNode *node, std::shared_ptr<ConversionContext> ctx) override;

        void ConvertToGameFormat(pugi::xml_node out, std::shared_ptr<ConversionContext> ctx) override;

    private:

    };

}


