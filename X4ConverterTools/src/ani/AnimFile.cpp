#include <X4ConverterTools/ani/AnimFile.h>


// Callee should free

using namespace boost;
using namespace Assimp;

namespace ani {
// TODO copy constructors?
    AnimFile::AnimFile(IOStream *pStream) {
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
            }
            catch (std::exception &e) {
                s.append(e.what());
                throw std::runtime_error(s);
            }
        }

        return s;
    }

    void AnimFile::WriteAnims(pugi::xml_node tgtNode) const {
        for (const AnimDesc &desc : descs) {
            if (desc.SafeSubName.compare(0, sizeof("landinggears_activating"), "landinggears_activating") == 0) {
                desc.WriteAnim(tgtNode);
            } else {
                std::cout << desc.SafeSubName << std::endl;
            }
        }

    }
}