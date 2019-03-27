#include <X4ConverterTools/Ani/AniFile.h>


// Callee should free

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniFile::AniFile(IOStream *pStream) {
    // TODO endian handling??
    // TODO pass this in instead of pstream?
    auto pStreamReader = StreamReaderLE(pStream, false);
    header = AniHeader(pStreamReader);
    descs = std::vector<AniAnimDesc>();
    for (int i = 0; i < header.getNumAnims(); i++) {
        descs.emplace_back(pStreamReader);
    }
    if (pStreamReader.GetCurrentPos() != header.getKeyOffsetBytes()) {
        std::string err = str(format("AniFile: current position (%1%) does not align with the data offset (%2%)") %
                              pStreamReader.GetCurrentPos() % header.getKeyOffsetBytes());
        throw std::runtime_error(err);
    }
    for (int i = 0; i < header.getNumAnims(); i++) {
        descs[i].read_frames(pStreamReader);
    }
    validate();
}
AniFile::~AniFile(){

}
AniHeader AniFile::getHeader() const {
    return header;
}


void AniFile::setHeader(AniHeader header) {
    AniFile::header = header;
}

std::string AniFile::validate(){
    std::string s;
    s.append(header.validate());
    for (int i = 0; i < descs.size(); i++){
        try {
            auto desc = descs[i];
            std::string ret = desc.validate();
            s.append(ret);
        }
        catch (std::exception &e){
            s.append(e.what());
            throw std::runtime_error(s);
        }
    }

    return s;
}

void AniFile::WriteAnims(pugi::xml_node tgtNode) const {
    for (const AniAnimDesc &desc : descs){
        desc.WriteAnim(tgtNode);
    }

}
