#pragma once

#include <assimp/StreamWriter.h>
#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <X4ConverterTools/ani/Keyframe.h>
#include <vector>
#include <set>
#include <iostream>
#include <pugixml.hpp>

namespace ani {
    class AnimDesc {
    public:
        AnimDesc() = default;

        AnimDesc(std::string partName, pugi::xml_node node);

        explicit AnimDesc(Assimp::StreamReaderLE &reader);

        void read_frames(Assimp::StreamReaderLE &reader);

        std::string validate();// Debug method - throws exception if invalid, else returns human readable string
        void WriteIntermediateRepr(pugi::xml_node tgtNode) const;

        void WriteIntermediateReprOfChannel(pugi::xml_node tgtNode, std::string keyType, std::string axis) const;

        void WriteToGameFiles(Assimp::StreamWriterLE &writer);

        std::string SafeSubName;
    protected:
        char Name[64] = {0};
        char SubName[64] = {0};
        std::string SafeName;
        int NumPosKeys = 0;
        int NumRotKeys = 0;
        int NumScaleKeys = 0;
        int NumPreScaleKeys = 0;
        int NumPostScaleKeys = 0;
        float Duration = 0;
        int Padding[2] = {0};
        std::vector<Keyframe> posKeys;
        std::vector<Keyframe> rotKeys;
        std::vector<Keyframe> scaleKeys;
        std::vector<Keyframe> preScaleKeys;
        std::vector<Keyframe> postScaleKeys;

    };
}