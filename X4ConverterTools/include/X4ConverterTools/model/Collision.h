#pragma once

#include <boost/format.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "Lod.h"

namespace model {

    const static int COLLISION_INDEX = -1;
    class Collision : public Lod {
    public:
        Collision(std::string partName);
        void ConvertFromAiNode(aiNode *node);
        void ConvertToXml(pugi::xml_node out);

    };

}


