#include <X4ConverterTools/ani/AnimFile.h>


// Callee should free

using namespace boost;
using namespace Assimp;

namespace ani {
// TODO copy constructors?
    AnimFile::AnimFile(IOStream *pStream) {        // TODO endian handling??
        // TODO pass this in instead of pstream?
        auto pStreamReader = StreamReaderLE(pStream, false);
        header = Header(pStreamReader);
        descs = std::vector<AnimDesc>();
        for (int i = 0; i < header.getNumAnims(); i++) {
            descs.emplace_back(pStreamReader);
        }
        if (pStreamReader.GetCurrentPos() != header.getKeyOffsetBytes()) {
            std::string err = str(format("AnimFile: current position (%1%) does not align with the data offset (%2%)") %
                                  pStreamReader.GetCurrentPos() % header.getKeyOffsetBytes());
            throw std::runtime_error(err);
        }
        for (int i = 0; i < header.getNumAnims(); i++) {
            descs[i].read_frames(pStreamReader);
        }
        validate();
        // TODO fixme
    }

    AnimFile::AnimFile(IOStream *pStream,const std::string& xmlPath) {
        // TODO endian handling??
        // TODO pass this in instead of pstream?
        auto pStreamReader = StreamReaderLE(pStream, false);
        header = Header(pStreamReader);
        descs = std::vector<AnimDesc>();
        for (int i = 0; i < header.getNumAnims(); i++) {
            descs.emplace_back(pStreamReader);
        }
        if (pStreamReader.GetCurrentPos() != header.getKeyOffsetBytes()) {
            std::string err = str(format("AnimFile: current position (%1%) does not align with the data offset (%2%)") %
                                  pStreamReader.GetCurrentPos() % header.getKeyOffsetBytes());
            throw std::runtime_error(err);
        }
        for (int i = 0; i < header.getNumAnims(); i++) {
            descs[i].read_frames(pStreamReader);
        }
        validate();
        // TODO more validation


        // TODO integrate into Component?

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(xmlPath.c_str());
        if (result.status != pugi::status_ok) {
            throw std::runtime_error(str(format("Failed to parse %s: %s") % xmlPath % result.description()));
        }
        pugi::xml_node componentNode = doc.select_node("/components/component").node();
        if (!componentNode) {
            throw std::runtime_error("File has no <component> element");
        }

        pugi::xpath_node_set partNodes = componentNode.select_nodes("connections/connection");
        for (auto partNode : partNodes) {
            std::string name = partNode.node().attribute("name").value();

            meta[name] = std::map<std::string, std::pair<int, int> >();
            pugi::xpath_node_set animMetas = partNode.node().select_nodes("animations/animation");
            for (auto animEntry : animMetas) {
                auto animMeta = animEntry.node();
                meta[name][animMeta.attribute("name").value()] = std::make_pair(animMeta.attribute("start").as_int(),
                                                                                  animMeta.attribute("end").as_int());
            }
        }
    }

    AnimFile::~AnimFile() = default;

    Header AnimFile::getHeader() const {
        return header;
    }


    void AnimFile::setHeader(Header header) {
        AnimFile::header = header;
    }

    std::string AnimFile::validate() {
        std::string s;
        s.append(header.validate());
        for (auto desc : descs) {
            try {
                std::string ret = desc.validate();
                s.append(ret);
            } catch (std::exception &e) {
                s.append(e.what());
                throw std::runtime_error(s);
            }
        }

        return s;
    }

    void AnimFile::WriteAnims(pugi::xml_node tgtNode) const {
        pugi::xml_node dataNode = tgtNode.append_child("data");
        for (const AnimDesc &desc : descs) {
//            if (desc.SafeSubName.compare(0, sizeof("landinggears_activating"), "landinggears_activating") == 0) {
                desc.WriteAnim(dataNode);
        }
        pugi::xml_node metaNode = tgtNode.append_child("metadata");
        for (auto entry : meta) {
            pugi::xml_node connNode = metaNode.append_child("connection");
            connNode.append_attribute("name").as_string(entry.first.c_str());
            for (auto subEntry : entry.second) {
                pugi::xml_node animNode = connNode.append_child("animation");
                animNode.append_attribute("animName").as_string(subEntry.first.c_str());
                animNode.append_attribute("start").as_int(subEntry.second.first);
                animNode.append_attribute("end").as_int(subEntry.second.first);
            }
        }

    }

}