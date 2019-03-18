#include <X4ConverterTools/Ani/AniItemDesc.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniItemDesc::AniItemDesc(StreamReader<>& reader) {
    char wordBuf0[64];
    char wordBuf1[64];
    for (char &c : wordBuf0){
        reader >> c;
    }


    for (char &c : wordBuf1){
        reader >> c;
    }
    for (unsigned char &i : padding0) {
        reader >> i;
    }

}



std::string AniItemDesc::validate() {
    bool valid = true;
    std::string ret = "Item Desc: \n";


    ret.append("Buffer: ");
    // TODO words
    auto fmt = format("%1$02x ");
    for (unsigned char &i : padding0) {
        std::string part = str(fmt % (int) i);
        ret.append(part);
    }

    ret.append("\n");

    if(!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}
