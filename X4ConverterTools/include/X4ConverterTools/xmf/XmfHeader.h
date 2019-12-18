#pragma once

#include <cstdint>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

namespace xmf {
class XmfHeader final {
 public:
  constexpr const static uint8_t kExpectedBinarySize = 0x40;// If it's not 0x40 we have an issue

  XmfHeader() = default;

  XmfHeader(uint8_t NumDataBuffer, uint8_t NumMaterials);

  explicit XmfHeader(Assimp::StreamReaderLE &reader);

  void Write(Assimp::StreamWriterLE &writer);

  [[nodiscard]] std::string validate() const;// Debug method - throws exception if invalid, else returns human readable string
  friend bool operator==(XmfHeader const &lhs, XmfHeader const &rhs);
  friend bool operator!=(XmfHeader const &lhs, XmfHeader const &rhs) {
    return !(lhs == rhs);
  }
  uint8_t Magic[4];                      // bytes   0 - 3; identifies the file
  uint8_t Version;                       // byte        4; file version
  uint8_t IsBigEndian;                   // byte        5; if == 0 we have little endian (default)
  uint8_t SizeOfHeader;                  // byte        6; sizeof(XU_MESH_FILE_HEADER), should be 64byte
  uint8_t reserved0 = 0;                 // byte        7; Officially reserved
  uint8_t NumDataBuffers;                // byte        8; how many databuffers this file has stored
  uint8_t DataBufferDescSize;            // byte        9; sizeof(DATA_BUFFER_DESC)
  uint8_t NumMaterials;                  // byte       10; how many attribute ranges we have stored TODO rename
  uint8_t MaterialSize;                  // byte       11; sizeof(ATTRIBUTE_TABLE_DESC) TODO rename

  uint8_t Culling_CW;                    // byte       12; false == CCW, other is CW TODO validate
  uint8_t
      RightHand;                     // byte       13; 0 == lefthand D3D convention, other is righthand openGL format todo validate
  uint32_t NumVertices;                  // bytes 14 - 17; TODO use for validation
  uint32_t NumIndices;                   // bytes 18 - 21; TODO use for validation

  uint32_t PrimitiveType;                // bytes 22 - 25;
  uint32_t MeshOptimization;             // bytes 26 - 29; MeshOptimizationType - enum MeshOptimizationType applied
  aiVector3D BoundingBoxCenter;          // bytes 30 - 41; virtual center of the mesh TODO calculate
  aiVector3D
      BoundingBoxSize;            // bytes 42 - 53; max absolute extents from the center (aligned coords) TODO calculate
  uint8_t pad[10];                       // bytes 54 - 63
};

}