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

        virtual aiNode *ConvertToAiNode();

        virtual void ConvertFromAiNode(aiNode *node);
        virtual void ConvertToXml(pugi::xml_node out);

        int getIndex();

    protected:
        int index;
    };

}


