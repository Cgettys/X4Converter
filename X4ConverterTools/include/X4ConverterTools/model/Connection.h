#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <vector>
#include <map>
#include "AbstractElement.h"

namespace model {
    class Connection : AbstractElement {
    public:
        Connection(pugi::xml_node node);

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);

        std::string getParentName();

    private:
        std::string parentName = "ROOT";
        std::vector<Part> parts;
        std::map<std::string, std::string> attrs;
        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    };
}