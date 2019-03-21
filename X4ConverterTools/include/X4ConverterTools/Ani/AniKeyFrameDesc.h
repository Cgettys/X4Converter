#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>

// TODO better name
class AniKeyFrameDesc {
public:
    AniKeyFrameDesc() = default;

    explicit AniKeyFrameDesc(Assimp::StreamReader<> &reader);

    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
protected:
    char Name[64];
    char SubName[64];
    int NumPosKeys;
    int NumRotKeys;
    int NumScaleKeys;
    int NumPreScaleKeys;
    int NumPostScaleKeys;
    float Duration;
    int Padding[2];
};
