#pragma once

#include <cstdint>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/xmf/XmfVertexElement.h>
#include <X4ConverterTools/util/DXUtil.h>
#include "IXmf.h"

namespace xmf {
class XmfDataBufferDesc : public IXmf {
 public:
  static constexpr const uint8_t kExpectedBinarySize = 0xBC;
  XmfDataBufferDesc() = default;

  explicit XmfDataBufferDesc(Assimp::StreamReaderLE &reader);

  void ReadBinary(Assimp::StreamReaderLE &reader) override;
  void WriteBinary(Assimp::StreamWriterLE &writer) const override;

  std::string validate();

  [[nodiscard]] uint64_t GetCompressedDataSize() const {
    return CompressedDataSize;
  }

  [[nodiscard]] uint64_t GetUncompressedDataSize() const {
    return NumSections * NumItemsPerSection * ItemSize;
  }

  [[nodiscard]] bool IsCompressed() const;

  [[nodiscard]] bool IsVertexBuffer() const;

  [[nodiscard]] bool IsIndexBuffer() const;

  int GetVertexDeclarationSize();

  D3DFORMAT GetIndexFormat();

  uint32_t NumItemsPerSection;
  uint32_t NumVertexElements;
  XmfVertexElement VertexElements[16];

  uint32_t DataOffset;
  uint32_t Compressed;
  uint32_t Type;
  uint32_t UsageIndex;
  uint8_t _pad0[4] = {0, 0, 0, 0};
  uint32_t Format;
  uint32_t CompressedDataSize;
  int ItemSize;
  uint32_t NumSections;
  uint8_t _pad1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  void DenormalizeVertexDeclaration();

  void NormalizeVertexDeclaration();

 private:
  void ReadBinaryImpl(Assimp::StreamReaderLE &reader);
};
}
