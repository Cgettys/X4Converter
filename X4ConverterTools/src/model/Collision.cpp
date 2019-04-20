#include "X4ConverterTools/model/Collision.h"

namespace model {
    Collision::Collision(std::string partName) {
        index = COLLISION_INDEX;
        setName(str(boost::format("%1%|collision") % partName));
    }
    void Collision::ConvertFromAiNode(aiNode *node) {
        std::string rawName = node->mName.C_Str();
        setName(rawName);
        // Parse out the index
        size_t pos = rawName.rfind("|collision");
        if (pos == std::string::npos) {
            throw std::runtime_error("this is not a collision mesh");
        }
        index = COLLISION_INDEX;
    }

    void Collision::ConvertToXml(pugi::xml_node out) {

    }
}