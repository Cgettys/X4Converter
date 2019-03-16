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
    return std::__cxx11::string();
}
