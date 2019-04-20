#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <vector>
#include <map>
#include "AbstractElement.h"

namespace model {
    class Connection : AbstractElement {
    public:
        Connection(pugi::xml_node node,std::string componentName="");

        aiNode *ConvertToAiNode();

        void ConvertFromAiNode(aiNode *node);

        void ConvertToXml(pugi::xml_node out);

        std::string getParentName();

    private:
        std::string parentName;
        std::vector<Part> parts;
        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    };
}