#include <X4ConverterTools/Ani/AniHeader.h>
using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniHeader::AniHeader() = default;

AniHeader::AniHeader(Assimp::StreamReader<>& reader) {
    reader >> numStates;
    reader >> padding0[0];
    reader >> padding0[1];

    reader >> stateDataOffset;
    reader >> padding1[0];
    reader >> padding1[1];
    reader >> usedUnknown1;// TODO figure this out; every known file has this as a 1
    for (unsigned char &i : padding2) {
        reader >> i;
    }
    validate();
}
// TODO Separate validation from string stuff
std::string AniHeader::validate() {
    std::string result;
    bool valid = true;
    result.append("Header data:\n");

    // Gotta love format strings
    result.append(str(format("numStates:  %1$d (%1$#04x)\n") % (unsigned int) numStates));

    // First set of unknown bytes
    result.append("padding0 (0x):");
    for (unsigned char x : padding0) {
        result.append(str(format(" %02x") % (int) x));
    }
    result.append("\n");
    result.append(str(format("stateDataOffset: %1$d (%1$#06x)\n") % (unsigned int) stateDataOffset));

    // Second set of unknown bytes
    result.append("padding1 (0x):");
    for (unsigned char x : padding1) {
        result.append(str(format(" %02x") % (int) x));// Ignore the int cast, format strings suck
    }
    result.append("\n");

    // Third set of unknown bytes
    result.append("padding2 (0x):");
    for (unsigned char x : padding2) {
        result.append(str(format(" %02x") % (int) x));// Ignore the int cast, format strings suck
    }
    result.append("\n");
    // Make sure padding bytes are actually padding bytes
    for (int x = 0; x < sizeof(padding0); x++){
        if (padding0[x]!= 0){
            std::string error = str(format("AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found at position %1% in set 0 of padding bytes\n") %x);
            result.append(error);
            valid=false;
        }
    }
    for (int x = 0; x < sizeof(padding1); x++){
        if (padding1[x]!= 0){
            std::string error = str(format("AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found at position %1% in set 1 of padding bytes\n") % x);
            result.append(error);
            valid=false;
        }
    }
    for (int x = 0; x < sizeof(padding2); x++){
        if (padding2[x]!= 0){
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

uint16_t AniHeader::getNumStates() const {
    return numStates;
}

void AniHeader::setNumStates(uint16_t numStates) {
    AniHeader::numStates = numStates;
}

uint16_t AniHeader::getStateDataOffset() const {
    return stateDataOffset;
}

void AniHeader::setStateDataOffset(uint16_t stateDataOffset) {
    AniHeader::stateDataOffset = stateDataOffset;
}

uint8_t AniHeader::getUsedUnknown1() const {
    return usedUnknown1;
}

void AniHeader::setUsedUnknown1(uint8_t usedUnknown1) {
    AniHeader::usedUnknown1 = usedUnknown1;
}
