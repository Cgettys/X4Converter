#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/Ani/AniKeyframe.h>
#include <vector>
#include <iostream>
// TODO better name
class AniAnimDesc {
public:
    AniAnimDesc() = default;

    explicit AniAnimDesc(Assimp::StreamReaderLE &reader);
    void read_frames(Assimp::StreamReaderLE &reader);
    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
protected:
    char Name[64];
    char SubName[64];
    std::string SafeName;
    std::string SafeSubName;
    int NumPosKeys;
    int NumRotKeys;
    int NumScaleKeys;
    int NumPreScaleKeys;
    int NumPostScaleKeys;
    float Duration;
    int Padding[2];
    std::vector<AniKeyframe> posKeys;
    std::vector<AniKeyframe> rotKeys;
    std::vector<AniKeyframe> scaleKeys;
    std::vector<AniKeyframe> preScaleKeys;
    std::vector<AniKeyframe> postScaleKeys;
    std::string validate_keyframe(AniKeyframe f);
};
