#pragma once

#include <map>
#include "zlib.h"
#include "../Vendor/half/half.hpp"
#include "X4ConverterTools/types.h"
#include <string>
#include <vector>


#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>

#include "X4ConverterTools/types/DirectX.h"
#include "../util/DXUtil.h"

#include <X4ConverterTools/types/ConversionContext.h>
#include "XmfHeader.h"
#include "XmfVertexElement.h"
#include "XmfMaterial.h"
#include "XmfDataBufferDesc.h"
#include "XmfDataBuffer.h"
    namespace xmf {

        class XuMeshFile {
        public:
            // TODO something better that takes into account writing?
            XmfHeader GetHeader() { return header; };

            std::vector<XmfDataBuffer> &GetBuffers() { return buffers; };

            XmfDataBuffer *GetIndexBuffer();

            std::vector<XmfVertexElement> GetVertexDeclaration();


            int NumVertices();

            int NumIndices();

            std::vector<XmfMaterial> &GetMaterials() { return materials; };

            int NumMaterials() { return materials.size(); }

            void AddMaterial(int firstIndex, int numIndices, const std::string &name);

            static std::shared_ptr<XuMeshFile> ReadFromFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

            static std::shared_ptr<XuMeshFile> ReadFromIOStream(Assimp::IOStream *pStream);

            void WriteToFile(const std::string &filePath, Assimp::IOSystem *pIOHandler);

            void WriteToIOStream(Assimp::IOStream *pStream);

            aiNode *ConvertXuMeshToAiNode( const std::string &name, ConversionContext &context);

            aiMesh *ConvertXuMeshToAiMesh(int firstIndex, int numIndices, const std::string &name,
                                          ConversionContext &context);

            //void                            AllocSceneMaterials             ( aiScene* pScene);
            void AllocMeshVertices(aiMesh *pMesh,  int numVertices);

            void AllocMeshFaces(aiMesh *pMesh, int numIndices);

            void PopulateMeshVertices(aiMesh *pMesh,int firstIndex, unsigned int numIndices);

            static void PopulateMeshFaces(aiMesh *pMesh, int numIndices);

        private:

            void ReadBufferDescs(Assimp::StreamReaderLE &pStreamReader);

            void ReadBuffers(Assimp::StreamReaderLE &pStream);

            void Validate();

            std::map<XmfDataBuffer *, std::vector<byte> > CompressBuffers();

            void WriteBufferDescs(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

            void WriteBuffers(Assimp::IOStream *pStream, std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers);

            XmfHeader header;
            std::vector<XmfDataBuffer> buffers;
            std::vector<XmfMaterial> materials;
        };

    }