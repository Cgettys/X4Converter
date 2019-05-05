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
        // TODO something better that takes into account writing?
        XmfHeader GetHeader() { return header; };

        std::vector<XmfDataBuffer> &GetBuffers() { return buffers; };

        XmfDataBuffer *GetIndexBuffer();

        std::vector<XmfVertexElement> GetVertexDeclaration();


        int NumVertices();

        int NumIndices();

        std::vector<XmfMaterial> &GetMaterials() { return materials; };

        int NumMaterials();

        void AddMaterial(int firstIndex, int numIndices, const std::string &name);

        static std::shared_ptr<XmfFile> ReadFromFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

        static std::shared_ptr<XmfFile> ReadFromIOStream(Assimp::IOStream *pStream);

        void WriteToFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

        void WriteToIOStream(Assimp::IOStream *pStream);

        aiNode *ConvertToAiNode(const std::string &name, std::shared_ptr<ConversionContext> ctx);

        aiMesh *ConvertToAiMesh(int firstIndex, int numIndices, const std::string &name,
                                std::shared_ptr<ConversionContext> context);

        //void                            AllocSceneMaterials             ( aiScene* pScene);
        void AllocMeshVertices(aiMesh *pMesh, int numVertices);

        static void AllocMeshFaces(aiMesh *pMesh, int numIndices);

        void PopulateMeshVertices(aiMesh *pMesh, int firstIndex, uint32_t numIndices);

        static void PopulateMeshFaces(aiMesh *pMesh, int numIndices);

        // For export:
        static std::shared_ptr<XmfFile> GenerateMeshFile(const aiScene *pScene, aiNode *pNode, bool isCollisionMesh);

        static void ExtendVertexDeclaration(aiMesh *pMesh, std::vector<XmfVertexElement> &declaration);

        static void ApplyVertexDeclaration(std::vector<XmfVertexElement> &declaration, XmfDataBuffer &buffer);

    private:

        void ReadBufferDescs(Assimp::StreamReaderLE &pStreamReader);

        void ReadBuffers(Assimp::StreamReaderLE &pStream);

        void Validate();

        std::map<XmfDataBuffer *, std::vector<uint8_t> > CompressBuffers();

        void WriteBufferDescs(Assimp::StreamWriterLE &pStreamWriter,
                              std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers);

        void WriteBuffers(Assimp::StreamWriterLE &pStreamWriter,
                          std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers);

        XmfHeader header;
        std::vector<XmfDataBuffer> buffers;
        std::vector<XmfMaterial> materials;
    };

}
