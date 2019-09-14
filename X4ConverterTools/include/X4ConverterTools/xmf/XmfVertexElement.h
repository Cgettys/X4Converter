#pragma once

#include <cstdint>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <assimp/mesh.h>

#include <X4ConverterTools/types.h>
#include <X4ConverterTools/util/DXUtil.h>
#include "IXmf.h"

namespace xmf {
class XmfVertexElement : public IXmf {

 public:
  XmfVertexElement();

  XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, uint8_t usageIndex = 0);

  explicit XmfVertexElement(Assimp::StreamReaderLE &reader);

  void ReadBinary(Assimp::StreamReaderLE &reader) override;
  void WriteBinary(Assimp::StreamWriterLE &writer) const override;

  int WriteVertexElement(aiMesh *pMesh, uint32_t vertexIdx, uint8_t *pElemData);

  uint32_t Type;
  uint8_t Usage;
  uint8_t UsageIndex;
  uint8_t _pad0[2] = {0, 0};
 private:
  void ReadBinaryImpl(Assimp::StreamReaderLE &reader);
};
}