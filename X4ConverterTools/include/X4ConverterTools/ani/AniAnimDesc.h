#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/ani/AniKeyframe.h>
#include <vector>
#include <iostream>
#include "../pugixml.hpp"

class AniAnimDesc {
public:
    AniAnimDesc() = default;

    explicit AniAnimDesc(Assimp::StreamReaderLE &reader);

    void read_frames(Assimp::StreamReaderLE &reader);

    std::string validate();// Debug method - throws exception if invalid, else returns human readable string
    void WriteAnim(pugi::xml_node tgtNode) const;

    void WriteChannel(pugi::xml_node tgtNode, std::string keyType, std::string axis) const;

    std::string SafeSubName;
protected:
    char Name[64];
    char SubName[64];
    std::string SafeName;
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

    void WriteInputElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                           std::string &axis) const;

    void WriteOutputElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                std::string &axis, std::string &keyType) const;

    void WriteInterpolationElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                   std::string &axis) const;

    void WriteInTangentElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                   std::string &axis) const;

    void WriteOutTangentElement(std::string &animKey, pugi::xml_node &animRoot, std::vector<AniKeyframe> &frames,
                                    std::string &axis) const;

    void WriteSamplerElement(std::string &animKey, pugi::xml_node &animRoot) const;
};
