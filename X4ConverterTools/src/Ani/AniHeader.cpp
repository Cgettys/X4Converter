#include <X4ConverterTools/Ani/AniHeader.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniHeader::AniHeader() = default;

AniHeader::AniHeader(Assimp::StreamReaderLE &reader) {
    reader >> NumAnims;

    reader >> KeyOffsetBytes;
    reader >> Version;
    reader >> Padding;
    validate();
}

// TODO Separate validation from string stuff
std::string AniHeader::validate() {
    std::string result;
    bool valid = true;
    result.append("Header data:\n");

    // Gotta love format strings
    result.append(str(format("\tNumAnims:  %1$d (%1$#04x)\n") % NumAnims));

    result.append(str(format("\tKeyOffsetBytes: %1$d (%1$#06x)\n") % KeyOffsetBytes));

    result.append(str(format("\tVersion: %1$d (%1$#06x)\n") %  Version));
    result.append(str(format("\tPadding: %1$d (%1$#06x)\n") %  Padding));
    if (Version != 1) {
        result.append(
                "Ani file format has been updated!\n");
        valid = false;
    }

    if (Padding != 0) {
        result.append(
                "AniHeader structure is no longer accurate or file is corrupted; Non-zero byte was found in padding bytes\n");
        valid = false;
    }


    if (!valid) {
        std::string finalError = "Error, invalid ANI file:\n";
        finalError.
                append(result);
        throw
                std::runtime_error(finalError);
    }
    return
            result;
}

int AniHeader::getNumAnims() const {
    return NumAnims;
}

void AniHeader::setNumAnims(int NumAnims) {
    AniHeader::NumAnims = NumAnims;
}

int AniHeader::getKeyOffsetBytes() const {
    return KeyOffsetBytes;
}

void AniHeader::setKeyOffsetBytes(int KeyOffsetBytes) {
    AniHeader::KeyOffsetBytes = KeyOffsetBytes;
}

int AniHeader::getVersion() const {
    return Version;
}

void AniHeader::setVersion(int Version) {
    AniHeader::Version = Version;
}

int AniHeader::getPadding() const {
    return Padding;
}

void AniHeader::setPadding(int Padding) {
    AniHeader::Padding = Padding;
}

