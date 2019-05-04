#include "X4ConverterTools/model/VisualLod.h"

namespace model {
    VisualLod::VisualLod(pugi::xml_node node, std::string partName, const ConversionContext &ctx) {

        if (std::string(node.name()) != "lod") {
            throw std::runtime_error("XML element must be a <lod> element!");
        }
        if (node.attribute("index").empty()) {
            throw std::runtime_error("VisualLod must have an index attribute!");
        }
        index = node.attribute("index").as_int();
        setName(str(boost::format("%1%|lod%2%") % partName % index));
    }

    aiNode *VisualLod::ConvertToAiNode(const ConversionContext &ctx) {
        return new aiNode(getName());
    }

    void VisualLod::ConvertFromAiNode(aiNode *node, const ConversionContext &ctx) {
        std::string rawName = node->mName.C_Str();
        setName(rawName);
        // Parse out the index
        size_t pos = rawName.rfind("|lod");
        if (pos == std::string::npos) {
            throw std::runtime_error("lod lacks index");
        }
        index = std::stoi(rawName.substr(pos + 4));
    }

    void VisualLod::ConvertToXml(pugi::xml_node out, const ConversionContext &ctx) {
        if (std::string(out.name()) != "lods") {
            throw std::runtime_error("Lods must be added to a lods node");
        }
        auto lodNode = out.find_child_by_attribute("lod", "index", std::to_string(index).c_str());
        if (lodNode.empty()) {
            auto previousLod = out.find_child_by_attribute("lod", "index", std::to_string(index - 1).c_str());
            auto nextLod = out.find_child_by_attribute("lod", "index", std::to_string(index + 1).c_str());
            // Ensure correct ordering
            if (!previousLod.empty()) {
                lodNode = out.insert_child_after("lod", previousLod);
            } else if (!nextLod.empty()) {
                lodNode = out.insert_child_before("lod", nextLod);
            } else {
                lodNode = out.append_child("lod");
            }
            WriteAttr(lodNode, "index", std::to_string(index));
            auto matsNode = Child(lodNode, "materials");
        }
    }

}