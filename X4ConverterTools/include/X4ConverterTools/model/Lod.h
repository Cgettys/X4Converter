#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"

namespace model {

    class Lod : public AbstractElement {
    public:
        Lod() = default;

        Lod(pugi::xml_node node, std::string partName);

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

        int getIndex();

    protected:
        int index = -2;
    };

}


