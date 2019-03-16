#include <X4ConverterTools/Ani/AniFile.h>
// Callee should free

using namespace boost;
//AniFile::AniFile(){
//}
AniFile::~AniFile(){
    delete header;
}
AniFile* AniFile::ReadFromIOStream(Assimp::IOStream *pStream) {
    AniFile *p = new AniFile();
    p->header= new AniHeader();

    pStream->Read(p->header, sizeof(*(p->header)), 1);
  

    return p;
}

// TODO Separate validationf rom string stuff
std::string AniFile::validate() {
    std::string result = "";
    bool valid = true;
    result.append("Header data:\n");
    result.append(str(format("Header length: %1%\n") % sizeof(header)));

    // Gotta love format strings
    result.append(str(format("numStates:  %1$d (%1$#04x)\n") % (unsigned int) (header)->numStates));

    // First set of unknown bytes
    result.append("padding0 (0x):");
    for (int x = 0; x < sizeof(header->padding0); x++){
        result.append(str(format(" %02x") % (int)  header->padding0[x]));
    }
    result.append("\n");
    result.append(str(format("stateDataOffset: %1$d (%1$#06x)\n") % (unsigned int) header->stateDataOffset));

    // Second set of unknown bytes
    result.append("padding1 (0x):");
    for (int x = 0; x < sizeof(header->padding1); x++){
        result.append(str(format(" %02x") % (int)  header->padding1[x]));// Ignore the int cast, format strings suck
    }
    result.append("\n");

    // Third set of unknown bytes
    result.append("padding2 (0x):");
    for (int x = 0; x < sizeof(header->padding2); x++){
        result.append(str(format(" %02x") % (int)  header->padding2[x]));// Ignore the int cast, format strings suck
    }
    result.append("\n");
    // Make sure padding bytes are actually padding bytes
    for (int x = 0; x < sizeof(header->padding0); x++){
        if (header->padding0[x]!= 0){
            std::string error = str(format("AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found at position %1% in set 0 of padding bytes\n") %x);
            result.append(error);
            valid=false;
        }
    }
    for (int x = 0; x < sizeof(header->padding1); x++){
        if (header->padding1[x]!= 0){
            std::string error = str(format("AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found at position %1% in set 1 of padding bytes\n") % x);
            result.append(error);
            valid=false;
        }
    }
    for (int x = 0; x < sizeof(header->padding2); x++){
        if (header->padding2[x]!= 0){
            std::string error = str(format("AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found at position %1% in set 2 of padding bytes\n") % x);
            result.append(error);
            valid=false;
        }
    }
    if (!valid){
        std::string finalError = "Error, invalid ANI file:\n";
        finalError.append(result);
        throw std::runtime_error(finalError);
    }
    return result;
}

AniHeader *AniFile::getHeader() const {
    return header;
}

void AniFile::setHeader(AniHeader *header) {
    AniFile::header = header;
}
