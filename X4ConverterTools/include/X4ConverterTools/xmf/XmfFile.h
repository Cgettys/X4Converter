#pragma once

#include <cstdint>
#include <map>
#include <zlib.h>
#include "../Vendor/half/include/half.hpp"
#include "X4ConverterTools/types.h"
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

#include <X4ConverterTools/ConversionContext.h>
#include "XmfHeader.h"
#include "XmfVertexElement.h"
#include "XmfMaterial.h"
#include "XmfDataBuffer.h"

namespace xmf {

class XmfFile {
 public:
  using Ptr = std::shared_ptr<XmfFile>;
  explicit XmfFile(ConversionContext::Ptr ctx);
  // TODO something better that takes into account writing?
  XmfHeader GetHeader() { return header; };

  std::vector<XmfDataBuffer> &GetBuffers() { return buffers; };

  XmfDataBuffer *GetIndexBuffer();

  std::vector<XmfVertexElement> GetVertexDeclaration();

  uint32_t NumVertices();

  uint32_t NumIndices();

  std::vector<XmfMaterial> &GetMaterials() { return materials; };

  uint32_t NumMaterials();
  static XmfFile::Ptr ReadFromFile(const std::string &name, const ConversionContext::Ptr &ctx);
  static XmfFile::Ptr ReadFromIOStream(Assimp::IOStream *pStream, const ConversionContext::Ptr &ctx);

  void WriteToFile(const std::string &name);
  void WriteToIOStream(Assimp::IOStream *pStream);

  aiNode *ConvertToAiNode(const std::string &name);

  aiMesh *ConvertToAiMesh(int firstIndex, int numIndices, const std::string &name);

  void AllocMeshVertices(aiMesh *pMesh, int numVertices);

  static void AllocMeshFaces(aiMesh *pMesh, int numIndices);

  void PopulateMeshVertices(aiMesh *pMesh, uint32_t firstIndex, uint32_t numIndices);

  static void PopulateMeshFaces(aiMesh *pMesh, int numIndices);

  // For export:
  static XmfFile::Ptr GenerateMeshFile(const ConversionContext::Ptr &ctx, aiNode *pNode, bool isCollisionMesh);

  static void ExtendVertexDeclaration(aiMesh *pMesh, std::vector<XmfVertexElement> &declaration);

  static void ApplyVertexDeclaration(std::vector<XmfVertexElement> &declaration, XmfDataBuffer &buffer);

 private:

  void ReadBufferDescs(Assimp::StreamReaderLE &pStreamReader);

  void ReadBuffers(Assimp::StreamReaderLE &pStream);

  std::string Validate();

  std::map<XmfDataBuffer *, std::vector<uint8_t> > CompressBuffers();

  void WriteBufferDescs(Assimp::StreamWriterLE &pStreamWriter,
                        std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers);

  void WriteBuffers(Assimp::StreamWriterLE &pStreamWriter,
                    std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers);
  ConversionContext::Ptr ctx;
  XmfHeader header;
  std::vector<XmfDataBuffer> buffers{};
  std::vector<XmfMaterial> materials{};
};

}
