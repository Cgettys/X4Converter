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
        descs.emplace_back(pStreamReader);
    }
    if (pStreamReader.GetCurrentPos() != header.getStateDataOffset()) {
        std::string err = str(format("AniFile: current position (%1%) does not align with the data offset (%2%)") %
                              pStreamReader.GetCurrentPos() % header.getStateDataOffset());
        throw std::runtime_error(err);
    }
//    if (pStreamReader.GetRemainingSize()!= 576 * header.getNumStates()){
//
//        std::string err = str(format("AniFile: remaining file length (%1%) does not align with the number of states (%2%)") %
//                                      pStreamReader.GetRemainingSize() % header.getNumStates());
//        throw std::runtime_error(err);
//    }
    std::cout << pStreamReader.GetRemainingSize() << "|" << header.getNumStates() << "/"<<pStreamReader.GetRemainingSize()/32 << std::endl;
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
        auto desc = descs[i];
        std::string ret = desc.validate();
        s.append(ret);
    }

    return s;
}