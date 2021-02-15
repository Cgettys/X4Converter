#pragma once

#include <cstdint>
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

  AnimDesc(const std::string &partName, pugi::xml_node node);

  // Read the metadata
  explicit AnimDesc(Assimp::StreamReaderLE &reader);
  // Read the actual keyframes
  void read_frames(Assimp::StreamReaderLE &reader);

  std::string validate();// Debug method - throws exception if invalid, else returns human readable string
  void WriteIntermediateRepr(pugi::xml_node tgtNode) const;

  void WriteIntermediateReprOfChannel(pugi::xml_node tgtNode, const std::string &keyType, std::string axis) const;

  // Write the metadata
  void WriteToGameFiles(Assimp::StreamWriterLE &writer);

  // Write the actual keyframes
  void write_frames(Assimp::StreamWriterLE &writer);
  std::string SafeName;
  std::string SafeSubName;

 protected:
  std::vector<Keyframe> posKeys;
  std::vector<Keyframe> rotKeys;
  std::vector<Keyframe> scaleKeys;
  std::vector<Keyframe> preScaleKeys;
  std::vector<Keyframe> postScaleKeys;
  static bool validateFrameType(std::string &ret,
                                std::string name,
                                size_t expectedFrameCount,
                                std::vector<Keyframe> frames);
 private:
  // Physical representation
  char Name[64] = {0};
  char SubName[64] = {0};
  int NumPosKeys = 0;
  int NumRotKeys = 0;
  int NumScaleKeys = 0;
  int NumPreScaleKeys = 0;
  int NumPostScaleKeys = 0;
  float Duration = 0;
  int Padding[2] = {0};
};
}