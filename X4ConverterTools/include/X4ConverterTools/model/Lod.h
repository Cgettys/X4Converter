#pragma once

#include <pugixml.hpp>
#include <iostream>
#include "AbstractElement.h"
namespace model{

    class Lod  : public AbstractElement {
    public:
        Lod() = default;
        Lod(pugi::xml_node node);

        virtual aiNode *ConvertToAiNode();

        virtual void ConvertFromAiNode(aiNode *);
    };{

}


