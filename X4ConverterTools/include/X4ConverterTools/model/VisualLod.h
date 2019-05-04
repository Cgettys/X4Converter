#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

    class VisualLod : public Lod {
    public:
        VisualLod() = default;

        VisualLod(pugi::xml_node node, std::string partName, const ConversionContext &ctx);

        aiNode *ConvertToAiNode(const ConversionContext &ctx) override;

        void ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) override;

        void ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) override;

    };

}


