#include <X4ConverterTools/Ani/AniAnimDesc.h>

using namespace boost;
using namespace Assimp;

// TODO copy constructors?
AniAnimDesc::AniAnimDesc(StreamReader<> &reader) {
    for (char &c : Name) {
        reader >> c;

    }

    for (char &c : SubName) {
        reader >> c;
    }

    for (int i = 0; i < 64; i++){
        char c = Name[i];
        if (c==' '){
            throw std::runtime_error("Did not expect spaces in names");
        }
        else if (c == '\0'){
            SafeName.insert(i,1, ' ');
        } else{
            SafeName.insert(i,1, c);
        }
    }
    algorithm::trim_right(SafeName);

    for (int i = 0; i < 64; i++){
        char c = SubName[i];
        if (c==' '){
            throw std::runtime_error("Did not expect spaces in names");
        }
        else if (c == '\0'){
            SafeSubName.insert(i,1, ' ');
        } else{
            SafeSubName.insert(i,1, c);
        }
    }
    algorithm::trim_right(SafeSubName);

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

void AniAnimDesc::read_frames(StreamReader<> &reader) {
    for (int i = 0; i < NumPosKeys; i++) {
        posKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumRotKeys; i++) {
        rotKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumScaleKeys; i++) {
        scaleKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumPreScaleKeys; i++) {
        preScaleKeys.emplace_back(reader);
    }
    for (int i = 0; i < NumPostScaleKeys; i++) {
        postScaleKeys.emplace_back(reader);
    }
}

std::string AniAnimDesc::validate() {
    bool valid = true;
    std::string ret = "\nAnimationDesc: \n";


    ret.append(str(format("\tName: %1%\n\tSubName: %2%\n") % SafeName % SafeSubName));


    // TODO better indentation
    //TODO check expected # of frames = actual length of array
    ret.append(str(format(
            "\tNumPosKeys: %1%\n\tNumRotKeys: %2%\n\tNumScaleKeys: %3%\n\tNumPreScaleKeys: %4%\n\tNumPostScaleKeys: %5%\n\tDuration: %6%\n") %
                   NumPosKeys % NumRotKeys % NumScaleKeys % NumPreScaleKeys % NumPostScaleKeys % Duration));
    for (int &i : Padding) {
        if (i != 0) {
            valid = false;
            ret.append("PADDING MUST BE 0");
        }
    }
    ret.append("Position Keyframes:\n");
    for (int i = 0; i < NumPosKeys; i++) {
        try {
            ret.append(posKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Rotation Keyframes:\n");
    for (int i = 0; i < NumRotKeys; i++) {
        try {
            ret.append(rotKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Scale Keyframes:\n");
    for (int i = 0; i < NumScaleKeys; i++) {
        try {
            ret.append(scaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("Prescale Keyframes:\n");
    for (int i = 0; i < NumPreScaleKeys; i++) {
        try {
            ret.append(preScaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }

    ret.append("PostScale Keyframes:\n");
    for (int i = 0; i < NumPostScaleKeys; i++) {
        try {
            ret.append(postScaleKeys[i].validate());
        }
        catch (std::exception &e) {
            ret.append(e.what());
            valid = false;
        }
    }
    if (NumPreScaleKeys || NumPostScaleKeys){
        ret.append("WARNING: Pre/Post scale keyframes are not yet supported by this converter");
    }
    if (NumPreScaleKeys != NumPostScaleKeys){
        ret.append("NumPreScaleKeys Must equal NumPostScaleKeys (Don't ask me why - the game says so)");
        valid=false;
    }
    if (!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}
