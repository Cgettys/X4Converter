#include <X4ConverterTools/Ani/AniItemDesc.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniItemDesc::AniItemDesc(StreamReader<>& reader) {
    for (unsigned char &i : padding0) {
        reader >> i;
    }
}



std::string AniItemDesc::validate() {
    std::string ret;
    auto fmt = format("%1$#04x");
    for (unsigned char &i : padding0) {
        ret.append(str(fmt % i));
    }

    return ret;
}
