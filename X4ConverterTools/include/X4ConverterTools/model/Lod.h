#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"
namespace model{

    class Lod  : public AbstractElement {
    public:
        Lod() = default;
        Lod(pugi::xml_node node,std::string parentName);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode * node);

        int getIndex();
    protected:
        int index;
    };

}


