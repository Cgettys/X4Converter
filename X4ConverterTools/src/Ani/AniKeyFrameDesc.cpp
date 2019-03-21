#include <X4ConverterTools/Ani/AniKeyFrameDesc.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniKeyFrameDesc::AniKeyFrameDesc(StreamReader<> &reader) {
    for (char &c : Name) {
        reader >> c;
    }

    for (char &c : SubName) {
        reader >> c;
    }

    reader >> NumPosKeys;
    reader >> NumRotKeys;
    reader >> NumScaleKeys;
    reader >> NumPreScaleKeys;
    reader >> NumPostScaleKeys;
    reader >> Duration;
    for (int &i : Padding) {
        reader >> i;
    }

}


std::string AniKeyFrameDesc::validate() {
    bool valid = true;
    std::string ret = "KeyFrameDesc: \n";

    // TODO name subname


    ret.append(str(format(
            "NumPosKeys: %1%\nNumRotKeys: %2%\nNumScaleKeys: %3%\nNumPreScaleKeys: %4%\nNumPostScaleKeys: %5%\nDuration: %6%\n") %
                   NumPosKeys % NumRotKeys % NumScaleKeys % NumPreScaleKeys % NumPostScaleKeys % Duration));
    for (int &i : Padding) {
        if (i != 0) {
            valid = false;
            ret.append("PADDING MUST BE 0");
        }
    }

    if (!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}
