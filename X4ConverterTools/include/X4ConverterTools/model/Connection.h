#pragma once

#include <X4ConverterTools/model/Part.h>
#include <pugixml.hpp>
#include <vector>
#include "AbstractElement.h"

namespace model {
    class Connection : public AbstractElement {
    public:
        Connection() = default;

        explicit Connection(pugi::xml_node node, std::string componentName = "");

        explicit Connection(aiNode *node, std::string componentName = "");

        aiNode *ConvertToAiNode() override;

        void ConvertFromAiNode(aiNode *node) override;

        void ConvertToXml(pugi::xml_node out) override;

        std::string getParentName();

        std::vector<Part> getParts(){
            return parts;
        }

    private:
        std::string parentName;
        std::vector<Part> parts;
        aiVector3D offsetPos;
        aiQuaternion offsetRot;
    };
}