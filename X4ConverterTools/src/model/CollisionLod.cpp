#include "X4ConverterTools/model/CollisionLod.h"
#include <iostream>
#include <boost/format.hpp>

namespace model {
    // Not really a LOD but it makes more sense if we pretend
    CollisionLod::CollisionLod(std::shared_ptr<ConversionContext> ctx) : Lod(ctx) {}

    CollisionLod::CollisionLod(std::string partName, std::shared_ptr<ConversionContext> ctx) : Lod(ctx) {
        index = COLLISION_INDEX;
        setName(str(boost::format("%1%-collision") % partName));
    }

    aiNode *CollisionLod::ConvertToAiNode() {
        return new aiNode(getName());
    }

    void CollisionLod::ConvertFromAiNode(aiNode *node) {
        std::string rawName = node->mName.C_Str();
        setName(rawName);
        // Parse out the index
        size_t pos = rawName.rfind("-collision");
        if (pos == std::string::npos) {
            throw std::runtime_error("this is not a collision mesh");
        }
        index = COLLISION_INDEX;
    }

    void CollisionLod::ConvertToGameFormat(pugi::xml_node out) {
        // TODO XMF
    }
}