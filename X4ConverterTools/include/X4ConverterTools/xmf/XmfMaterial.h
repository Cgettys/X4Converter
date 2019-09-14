#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <boost/format.hpp>
#include "IXmf.h"

namespace xmf {
class XmfMaterial : public IXmf {
 public:
  constexpr const static uint8_t kExpectedBinarySize = 0x88;
  constexpr const static uint8_t kMaxStrLen = 128;
  XmfMaterial();

  XmfMaterial(uint32_t firstIndex, uint32_t numIndices, std::string name);

  explicit XmfMaterial(Assimp::StreamReaderLE &reader);
  void ReadBinary(Assimp::StreamReaderLE &reader) override;
  void WriteBinary(Assimp::StreamWriterLE &writer) const override;

  uint32_t FirstIndex;
  uint32_t NumIndices;
  char Name[kMaxStrLen];
 private:
  inline void ReadBinaryImpl(Assimp::StreamReaderLE &reader);
};
}