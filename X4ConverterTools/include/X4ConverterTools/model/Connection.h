#pragma once

#include <pugixml.hpp>
#include "AbstractElement.h"

namespace model {
    class Connection : AbstractElement{
    public:
        Connection(pugi::xml_node node);

        virtual aiNode *ConvertToAiNode();

        virtual void ConvertFromAiNode(aiNode * node);
    protected:
        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    };
}