#include <X4ConverterTools/xmf/XmfImporter.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>

using namespace Assimp;
using namespace boost;
    namespace xmf {
        XmfDataBuffer *XuMeshFile::GetIndexBuffer() {
            for (auto &_buffer : buffers) {
                if (_buffer.IsIndexBuffer())
                    return &_buffer;
            }
            return nullptr;
        }

        std::vector<XmfVertexElement> XuMeshFile::GetVertexDeclaration() {
            std::vector<XmfVertexElement> result;
            for (auto &_buffer : buffers) {
                for (int i = 0; i < _buffer.Description.NumVertexElements; ++i) {
                    result.push_back(_buffer.Description.VertexElements[i]);
                }
            }
            return result;
        }

        int XuMeshFile::NumVertices() {
            for (auto &_buffer : buffers) {
                if (_buffer.IsVertexBuffer())
                    return _buffer.Description.NumItemsPerSection;
            }
            return 0;
        }

        int XuMeshFile::NumIndices() {
            XmfDataBuffer *pIndexBuffer = GetIndexBuffer();
            return pIndexBuffer != nullptr ?
                   pIndexBuffer->Description.NumItemsPerSection : 0;
        }

        void XuMeshFile::AddMaterial(int firstIndex, int numIndices,
                                     const std::string &name) {
            materials.emplace_back(firstIndex, numIndices, name);
        }

        std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromFile(
                const std::string &filePath, Assimp::IOSystem *pIOHandler) {
            IOStream *pStream;
            try {
                pStream = pIOHandler->Open(filePath, "rb");
                auto result = XuMeshFile::ReadFromIOStream(pStream);
//        pIOHandler->Close(pStream);
                return result;
            } catch (...) {
                throw;
            }
        }

        std::shared_ptr<XuMeshFile> XuMeshFile::ReadFromIOStream(IOStream *pStream) {
            if (!pStream) {
                throw std::runtime_error("pStream may not be null!");
            }
            try {
                std::shared_ptr<XuMeshFile> pMeshFile = std::make_shared<XuMeshFile>();

                if (pStream->FileSize() < 16) {
                    throw std::runtime_error(".xmf file is too small (< 16 bytes)");
                }

                auto pStreamReader = StreamReaderLE(pStream, false);
                pMeshFile->header = XmfHeader(pStreamReader);

                // Read in Buffer descs
                if (pStream->FileSize() < pMeshFile->header.SizeOfHeader
                                          + pMeshFile->header.NumDataBuffers * pMeshFile->header.DataBufferDescSize) {
                    throw std::runtime_error(".xmf file is too small based on header");
                }

                pMeshFile->ReadBufferDescs(pStreamReader);
                // Materials
                for (int i = 0; i < pMeshFile->header.NumMaterials; i++) {
                    pMeshFile->materials.emplace_back(pStreamReader);
                }

                // Buffers
                pMeshFile->ReadBuffers(pStreamReader);
                pMeshFile->Validate();

                return pMeshFile;
            } catch (...) {
                throw;
            }
        }

        void XuMeshFile::ReadBufferDescs(StreamReaderLE &pStreamReader) {


//    pStreamReader.SetCurrentPos(header.SizeOfHeader);
            for (int x = 0; x < header.NumDataBuffers; x++) {
                buffers.emplace_back(pStreamReader);
            }
        }

        void XuMeshFile::ReadBuffers(StreamReaderLE &pStreamReader) {
            for (XmfDataBuffer &buffer : buffers) {
                buffer.Read(pStreamReader);
            }
        }

        void XuMeshFile::Validate() {
            header.validate();
            int numVertices = -1;
            for (XmfDataBuffer &buffer : buffers) {
                if (!buffer.IsVertexBuffer()) {
                    continue;
                }
                if (numVertices == -1) {
                    numVertices = buffer.Description.NumItemsPerSection;
                } else if (buffer.Description.NumItemsPerSection != numVertices) {
                    throw std::runtime_error(
                            "Inconsistent vertex count across vertex buffers");
                }
            }
        }

        void XuMeshFile::WriteToFile(const std::string &filePath,
                                     IOSystem *pIOHandler) {
            IOStream *pStream = pIOHandler->Open(filePath, "wb+");
            if (!pStream) {
                throw std::runtime_error((format("Failed to open %1% for writing") % filePath.c_str()).str());
            }
            WriteToIOStream(pStream);
            // pStream is "Helpfully" closed by Assimp's StreamWriter
        }

        void XuMeshFile::WriteToIOStream(IOStream *pStream) {
            std::map<XmfDataBuffer *, std::vector<byte> > compressedBuffers =
                    CompressBuffers();

            header = XmfHeader(buffers.size(), materials.size());
            auto pStreamWriter = StreamWriterLE(pStream, false);
            header.Write(pStreamWriter);
            pStreamWriter.Flush();

            WriteBufferDescs(pStream, compressedBuffers);
            for (XmfMaterial &material: materials) {
                material.Write(pStreamWriter);
            }
            pStreamWriter.Flush();
            WriteBuffers(pStream, compressedBuffers);
        }

        std::map<XmfDataBuffer *, std::vector<byte> > XuMeshFile::CompressBuffers() {
            std::map<XmfDataBuffer *, std::vector<byte> > result;
            for (XmfDataBuffer &buffer: buffers) {
                std::vector<byte> &compressedData = result[&buffer];
                compressedData.resize(compressBound(buffer.GetUncompressedDataSize()));

                unsigned long int compressedSize = compressedData.size();
                int status = compress(compressedData.data(), &compressedSize,
                                      buffer.GetData(), buffer.GetUncompressedDataSize());
                if (status != Z_OK)
                    throw std::runtime_error("Failed to compress XMF data buffer");

                compressedData.resize(compressedSize);
            }
            return result;
        }


        void XuMeshFile::WriteBufferDescs(Assimp::IOStream *pStream,
                                          std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers) {
            int dataOffset = 0;
            for (XmfDataBuffer &buffer: buffers) {
                buffer.Description.DataOffset = dataOffset;
                buffer.Description.Compressed = 1;
                buffer.Description.CompressedDataSize =
                        compressedBuffers[&buffer].size();
                pStream->Write(&buffer.Description, sizeof(buffer.Description), 1);
                dataOffset += buffer.Description.CompressedDataSize;
            }
        }

        void XuMeshFile::WriteBuffers(Assimp::IOStream *pStream,
                                      std::map<XmfDataBuffer *, std::vector<byte> > &compressedBuffers) {
            for (XmfDataBuffer &buffer: buffers) {
                std::vector<byte> &compressedData = compressedBuffers[&buffer];
                pStream->Write(compressedData.data(), 1, compressedData.size());
            }
        }

        aiNode *XuMeshFile::ConvertXuMeshToAiNode(
                                                  const std::string &name, ConversionContext &context) {
            auto *pMeshGroupNode = new aiNode();
            pMeshGroupNode->mName = name;
            try {
                if (GetMaterials().empty()) {
                    aiMesh *pMesh = ConvertXuMeshToAiMesh( 0, NumIndices(),
                                                          name, context);

                    pMeshGroupNode->mMeshes = new dword[1];
                    pMeshGroupNode->mMeshes[pMeshGroupNode->mNumMeshes++] =
                            context.Meshes.size();
                    context.Meshes.push_back(pMesh);
                } else {
                    pMeshGroupNode->mChildren = new aiNode *[NumMaterials()];

                    for (XmfMaterial &material : GetMaterials()) {
                        aiMesh *pMesh = ConvertXuMeshToAiMesh(material.FirstIndex,
                                                              material.NumIndices,
                                                              name + "X"
                                                              + replace_all_copy(std::string(material.Name),
                                                                                 ".", "X"), context);

                        auto itMat = context.Materials.find(material.Name);
                        if (itMat == context.Materials.end()) {
                            pMesh->mMaterialIndex = context.Materials.size();
                            context.Materials[material.Name] = pMesh->mMaterialIndex;
                        } else {
                            pMesh->mMaterialIndex = itMat->second;
                        }

                        auto *pMeshNode = new aiNode();
                        pMeshNode->mName = pMesh->mName;
                        pMeshNode->mMeshes = new dword[1];
                        pMeshNode->mMeshes[pMeshNode->mNumMeshes++] =
                                context.Meshes.size();
                        context.Meshes.push_back(pMesh);

                        pMeshGroupNode->mChildren[pMeshGroupNode->mNumChildren++] =
                                pMeshNode;
                    }
                }
            } catch (...) {
                delete pMeshGroupNode;
                throw;
            }
            return pMeshGroupNode;
        }

        aiMesh *XuMeshFile::ConvertXuMeshToAiMesh(int firstIndex,
                                                  int numIndices, const std::string &name, ConversionContext &context) {
            auto *pMesh = new aiMesh();
            pMesh->mName = name;
            try {
                AllocMeshVertices(pMesh, numIndices);
                AllocMeshFaces(pMesh, numIndices);
                PopulateMeshVertices(pMesh, firstIndex, numIndices);
                PopulateMeshFaces(pMesh, numIndices);
            } catch (...) {
                delete pMesh;
                throw;
            }
            return pMesh;
        }

        void XuMeshFile::AllocMeshVertices(aiMesh *pMesh,
                                           int numVertices) {
            if (numVertices <= 0) {
                throw std::runtime_error("AllocMeshVertices: numVertices must be > 0");
            }

            for (XmfDataBuffer &buffer : GetBuffers()) {
                if (!buffer.IsVertexBuffer()) {
                    continue;
                }
                for (int i = 0; i < buffer.Description.NumVertexElements; ++i) {
                    XmfVertexElement &vertexElem = buffer.Description.VertexElements[i];
                    switch (vertexElem.Usage) {
                        case D3DDECLUSAGE_POSITION:
                            if (!pMesh->mVertices) {
                                pMesh->mVertices = new aiVector3D[numVertices];
                            } else {
                                throw std::runtime_error(
                                        "Duplicate POSITION vertex element");
                            }
                            break;

                        case D3DDECLUSAGE_NORMAL:
                            if (!pMesh->mNormals) {
                                pMesh->mNormals = new aiVector3D[numVertices];
                            } else {
                                throw std::runtime_error("Duplicate NORMAL vertex element");
                            }
                            break;

                        case D3DDECLUSAGE_TANGENT:
                            if (vertexElem.UsageIndex == 0) {
                                if (!pMesh->mTangents) {
                                    pMesh->mTangents = new aiVector3D[numVertices];
                                } else {
                                    throw std::runtime_error(
                                            "Duplicate TANGENT vertex element with usage index 0");
                                }
                            } else if (vertexElem.UsageIndex == 1) {
                                if (!pMesh->mBitangents) {
                                    pMesh->mBitangents = new aiVector3D[numVertices];
                                } else {
                                    throw std::runtime_error(
                                            "Duplicate TANGENT vertex element with usage index 1");
                                }
                            } else if (vertexElem.UsageIndex > 1) {
                                throw std::runtime_error(
                                        "Invalid usage index for TANGENT vertex element");
                            }
                            break;

                        case D3DDECLUSAGE_TEXCOORD:
                            if (vertexElem.UsageIndex >= AI_MAX_NUMBER_OF_TEXTURECOORDS) {
                                throw std::runtime_error(
                                        "Invalid usage index for TEXCOORD vertex element");
                            }
                            if (!pMesh->mTextureCoords[vertexElem.UsageIndex]) {

                                pMesh->mTextureCoords[vertexElem.UsageIndex] =
                                        new aiVector3D[numVertices];
                                pMesh->mNumUVComponents[vertexElem.UsageIndex] = 2;
                            } else {
                                throw std::runtime_error("Duplicate TEXCOORD element");
                            }
                            break;

                        case D3DDECLUSAGE_COLOR:
                            if (vertexElem.UsageIndex >= AI_MAX_NUMBER_OF_COLOR_SETS) {
                                throw std::runtime_error(
                                        "Invalid usage index for COLOR vertex element");
                            }
                            if (!pMesh->mColors[vertexElem.UsageIndex]) {
                                pMesh->mColors[vertexElem.UsageIndex] =
                                        new aiColor4D[numVertices];
                            } else {
                                throw std::runtime_error("Duplicate COLOR element");
                            }
                            break;

                        default:
                            throw std::runtime_error(str(format("Unexpected Usage: %1%") % vertexElem.Usage));
                    }
                }
            }
        }

        void XuMeshFile::AllocMeshFaces(aiMesh *pMesh, int numIndices) {
            if (numIndices <= 0) {
                throw std::runtime_error("AllocMeshFaces: numIndices must be > 0");
            }
            if (numIndices % 3) {
                throw std::runtime_error(
                        "AllocMeshFaces: numIndices must be a multiple of 3");
            }
            pMesh->mFaces = new aiFace[numIndices / 3];
        }

        void XuMeshFile::PopulateMeshVertices(aiMesh *pMesh, int firstIndex, unsigned int numIndices) {
            XmfDataBuffer *pIndexBuffer = GetIndexBuffer();
            if (!pIndexBuffer) {
                throw std::runtime_error("Mesh file has no index buffer");
            }
            if (numIndices <= 0) {
                throw std::runtime_error(
                        "PopulateMeshVertices: numIndices must be > 0");
            }
            if (firstIndex < 0) {
                throw std::runtime_error(
                        "PopulateMeshVertices: firstIndex must be >= 0");
            }
            if (firstIndex + numIndices
                > pIndexBuffer->Description.NumItemsPerSection) {
                throw std::runtime_error(
                        "PopulateMeshVertices: numIndices is too large");
            }
            byte *pIndexes = pIndexBuffer->GetData();
            D3DFORMAT indexFormat = pIndexBuffer->GetIndexFormat();

            for (XmfDataBuffer &buffer : GetBuffers()) {
                if (!buffer.IsVertexBuffer())
                    continue;

                byte *pVertexBuffer = buffer.GetData();
                int elemOffset = 0;
                int declarationSize = buffer.Description.ItemSize;

                for (int elemIdx = 0; elemIdx < buffer.Description.NumVertexElements;
                     ++elemIdx) {
                    XmfVertexElement &elem = buffer.Description.VertexElements[elemIdx];
                    auto elemType = (D3DDECLTYPE) elem.Type;

                    for (int vertexIdxIdx = firstIndex;
                         vertexIdxIdx < firstIndex + numIndices; ++vertexIdxIdx) {
                        int vertexIdx = (
                                indexFormat == D3DFMT_INDEX16 ?
                                ((word *) pIndexes)[vertexIdxIdx] :
                                ((int *) pIndexes)[vertexIdxIdx]);
                        if (vertexIdx < 0
                            || vertexIdx >= buffer.Description.NumItemsPerSection)
                            throw std::runtime_error(
                                    "PopulateMeshVertices: invalid index");

                        byte *pVertexElemData = pVertexBuffer
                                                + vertexIdx * declarationSize + elemOffset;
                        int localVertexIdx = vertexIdxIdx - firstIndex;
                        switch (elem.Usage) {
                            case D3DDECLUSAGE_POSITION: {
                                aiVector3D position = util::DXUtil::ConvertVertexAttributeToVec3D(
                                        pVertexElemData, elemType);
                                position.x = -position.x;
                                pMesh->mVertices[localVertexIdx] = position;
                                break;
                            }

                            case D3DDECLUSAGE_NORMAL: {
                                aiVector3D normal = util::DXUtil::ConvertVertexAttributeToVec3D(
                                        pVertexElemData, elemType);
                                normal.x = -normal.x;
                                pMesh->mNormals[localVertexIdx] = normal;
                                break;
                            }

                            case D3DDECLUSAGE_TANGENT: {
                                aiVector3D *pTangents = (
                                        elem.UsageIndex == 0 ?
                                        pMesh->mTangents : pMesh->mBitangents);
                                aiVector3D tangent = util::DXUtil::ConvertVertexAttributeToVec3D(
                                        pVertexElemData, elemType);
                                tangent.x = -tangent.x;
                                pTangents[localVertexIdx] = tangent;
                                break;
                            }

                            case D3DDECLUSAGE_TEXCOORD: {
                                aiVector3D texcoord = util::DXUtil::ConvertVertexAttributeToVec3D(
                                        pVertexElemData, elemType);
                                texcoord.y = 1.0f - texcoord.y;
                                pMesh->mTextureCoords[elem.UsageIndex][localVertexIdx] =
                                        texcoord;
                                break;
                            }

                            case D3DDECLUSAGE_COLOR: {
                                pMesh->mColors[elem.UsageIndex][localVertexIdx] =
                                        util::DXUtil::ConvertVertexAttributeToColorF(
                                                pVertexElemData, elemType);
                                break;
                            }

                            default:
                                throw std::runtime_error(str(format("Unexpected Usage: %1%") % elem.Usage));
                        }
                    }
                    elemOffset += util::DXUtil::GetVertexElementTypeSize(
                            (D3DDECLTYPE) elem.Type);
                }
            }
            pMesh->mNumVertices = numIndices;
        }

        void XuMeshFile::PopulateMeshFaces(aiMesh *pMesh, int numIndices) {
            int index = 0;
            for (int i = 0; i < numIndices / 3; ++i) {
                aiFace &face = pMesh->mFaces[pMesh->mNumFaces++];
                face.mIndices = new dword[3];
                while (face.mNumIndices < 3) {
                    face.mIndices[face.mNumIndices++] = index++;
                }
            }
        }
    }
