#include <X4ConverterTools/ani/Header.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
namespace ani {

    Header::Header(Assimp::StreamReaderLE &reader) {
        reader >> NumAnims;

        reader >> KeyOffsetBytes;
        reader >> Version;
        reader >> Padding;
        validate();
    }

// TODO Separate validation from string stuff
    std::string Header::validate() {
        std::string result;
        bool valid = true;
        result.append("Header data:\n");

        // Gotta love format strings
        result.append(str(format("\tNumAnims:  %1$d (%1$#04x)\n") % NumAnims));

        result.append(str(format("\tKeyOffsetBytes: %1$d (%1$#06x)\n") % KeyOffsetBytes));

        result.append(str(format("\tVersion: %1$d (%1$#06x)\n") % Version));
        result.append(str(format("\tPadding: %1$d (%1$#06x)\n") % Padding));
        if (Version != 1) {
            result.append("Ani file format has been updated!\n");
            valid = false;
        }
        if (KeyOffsetBytes < 16) {

            result.append("Ani file either is corrupted or the field is actually unsigned\n");
            valid = false;
        }

        if (Padding != 0) {
            result.append(
                    "Header structure is no longer accurate or file is corrupted; Non-zero byte was found in padding bytes\n");
            valid = false;
        }


        if (!valid) {
            std::string finalError = "Error, invalid ANI file:\n";
            finalError.append(result);
            throw std::runtime_error(finalError);
        }
        return result;
    }

    int Header::getNumAnims() const {
        return NumAnims;
    }

    void Header::setNumAnims(int NumAnims) {
        Header::NumAnims = NumAnims;
    }

    int Header::getKeyOffsetBytes() const {
        return KeyOffsetBytes;
    }

    void Header::setKeyOffsetBytes(int KeyOffsetBytes) {
        Header::KeyOffsetBytes = KeyOffsetBytes;
    }

    int Header::getVersion() const {
        return Version;
    }

    void Header::setVersion(int Version) {
        Header::Version = Version;
    }

    int Header::getPadding() const {
        return Padding;
    }

    void Header::setPadding(int Padding) {
        Header::Padding = Padding;
    }

}