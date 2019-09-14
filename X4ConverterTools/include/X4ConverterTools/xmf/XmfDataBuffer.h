#pragma once

#include <cstdint>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <X4ConverterTools/types.h>
#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>
#include "IXmf.h"

namespace xmf {
class XmfDataBuffer : public IXmf {
 public:
  XmfDataBuffer();

  XmfDataBufferDesc Description;

  explicit XmfDataBuffer(Assimp::StreamReaderLE &descReader);

  void ReadBinary(Assimp::StreamReaderLE &reader) override;

  void WriteBinary(Assimp::StreamWriterLE &writer) const override;

  bool IsIndexBuffer() const;

  bool IsVertexBuffer() const;

  uint8_t *GetData();

  D3DFORMAT GetIndexFormat();

  void AllocData();

 private:
  std::vector<uint8_t> _data;
};
}