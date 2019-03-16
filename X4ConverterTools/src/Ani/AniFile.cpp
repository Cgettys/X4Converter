#include <X4ConverterTools/Ani/AniFile.h>


// Callee should free

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniFile::AniFile(IOStream *pStream) {
    // TODO endian handling??
    // TODO pass this in instead of pstream?
    auto pStreamReader = StreamReader<>(pStream, false);
    header = AniHeader(pStreamReader);
    descs = std::vector<AniItemDesc>();
    for (int i = 0; i < header.getNumStates(); i++) {
        AniItemDesc desc(pStreamReader);
        descs.emplace_back(desc);
    }
    if (pStreamReader.GetCurrentPos() != header.getStateDataOffset()) {
        std::string err = str(format("AniFile: current position (%1%) does not align with the data offset (%2%)") %
                              pStreamReader.GetCurrentPos() % header.getStateDataOffset());
        throw std::runtime_error(err);
    }
    validate();
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
    for (auto desc : descs){
        s.append(desc.validate());
    }



    return s;
}