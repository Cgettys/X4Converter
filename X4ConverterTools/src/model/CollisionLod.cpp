#include "X4ConverterTools/model/CollisionLod.h"
#include <iostream>
#include <boost/format.hpp>

namespace model {
    // Not really a LOD but it makes more sense if we pretend
    CollisionLod::CollisionLod(std::string partName, const ConversionContext &ctx) {
        index = COLLISION_INDEX;
        setName(str(boost::format("%1%|collision") % partName));
    }

    aiNode *CollisionLod::ConvertToAiNode(const ConversionContext &ctx) {
        return new aiNode(getName());
    }

    void CollisionLod::ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) {
        std::string rawName = node->mName.C_Str();
        setName(rawName);
        // Parse out the index
        size_t pos = rawName.rfind("|collision");
        if (pos == std::string::npos) {
            throw std::runtime_error("this is not a collision mesh");
        }
        index = COLLISION_INDEX;
    }

    void CollisionLod::ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) {
        // TODO XMF
    }
}