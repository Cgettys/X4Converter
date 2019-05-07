#include <X4ConverterTools/xmf/XmfFile.h>

#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>
#include <regex>
#include <boost/numeric/conversion/cast.hpp>

using namespace Assimp;
using namespace boost;
using util::DXUtil;
namespace xmf {
    XmfDataBuffer *XmfFile::GetIndexBuffer() {
        for (auto &_buffer : buffers) {
            if (_buffer.IsIndexBuffer())
                return &_buffer;
        }
        return nullptr;
    }

    std::vector<XmfVertexElement> XmfFile::GetVertexDeclaration() {
        std::vector<XmfVertexElement> result;
        for (auto &_buffer : buffers) {
            for (int i = 0; i < _buffer.Description.NumVertexElements; ++i) {
                result.push_back(_buffer.Description.VertexElements[i]);
            }
        }
        return result;
    }

    int XmfFile::NumVertices() {
        for (auto &_buffer : buffers) {
            if (_buffer.IsVertexBuffer())
                return _buffer.Description.NumItemsPerSection;
        }
        return 0;
    }

    int XmfFile::NumIndices() {
        XmfDataBuffer *pIndexBuffer = GetIndexBuffer();
        return pIndexBuffer != nullptr ? pIndexBuffer->Description.NumItemsPerSection : 0;
    }

    int XmfFile::NumMaterials() {
        return boost::numeric_cast<int>(materials.size());
    }

    std::shared_ptr<XmfFile> XmfFile::ReadFromIOStream(IOStream *pStream) {
        if (!pStream) {
            throw std::runtime_error("pStream may not be null!");
        }
        try {
            std::shared_ptr<XmfFile> pMeshFile = std::make_shared<XmfFile>();

            if (pStream->FileSize() < 16) {
                throw std::runtime_error(".xmf file is too small (< 16 bytes)");
            }

            auto pStreamReader = StreamReaderLE(pStream, false);
            pMeshFile->header = XmfHeader(pStreamReader);

            // Read in Buffer descs
            if (pStream->FileSize() < pMeshFile->header.SizeOfHeader +
                                      pMeshFile->header.NumDataBuffers * pMeshFile->header.DataBufferDescSize) {
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

    void XmfFile::ReadBufferDescs(StreamReaderLE &pStreamReader) {


//    pStreamReader.SetCurrentPos(header.SizeOfHeader);
        for (int x = 0; x < header.NumDataBuffers; x++) {
            buffers.emplace_back(pStreamReader);
        }
    }

    void XmfFile::ReadBuffers(StreamReaderLE &pStreamReader) {
        for (XmfDataBuffer &buffer : buffers) {
            buffer.Read(pStreamReader);
        }
    }

    void XmfFile::Validate() {
        header.validate();
        int numVertices = -1;
        for (XmfDataBuffer &buffer : buffers) {
            if (!buffer.IsVertexBuffer()) {
                continue;
            }
            if (numVertices == -1) {
                numVertices = buffer.Description.NumItemsPerSection;
            } else if (buffer.Description.NumItemsPerSection != numVertices) {
                throw std::runtime_error("Inconsistent vertex count across vertex buffers");
            }
        }
    }


    void XmfFile::WriteToIOStream(IOStream *pStream) {
        std::map<XmfDataBuffer *, std::vector<uint8_t> > compressedBuffers = CompressBuffers();

        header = XmfHeader(numeric_cast<uint8_t>(buffers.size()), numeric_cast<uint8_t>(materials.size()));
        auto pStreamWriter = StreamWriterLE(pStream, false);
        header.Write(pStreamWriter);

        WriteBufferDescs(pStreamWriter, compressedBuffers);
        for (XmfMaterial &material: materials) {
            material.Write(pStreamWriter);
        }
        WriteBuffers(pStreamWriter, compressedBuffers);
    }

    std::map<XmfDataBuffer *, std::vector<uint8_t> > XmfFile::CompressBuffers() {
        std::map<XmfDataBuffer *, std::vector<uint8_t> > result;
        for (XmfDataBuffer &buffer: buffers) {
            std::vector<uint8_t> &compressedData = result[&buffer];
            compressedData.resize(compressBound(buffer.Description.GetUncompressedDataSize()));

            unsigned long int compressedSize = compressedData.size();
            int status = compress(compressedData.data(), &compressedSize, buffer.GetData(),
                                  buffer.Description.GetUncompressedDataSize());
            if (status != Z_OK) {
                throw std::runtime_error("Failed to compress XMF data buffer");
            }
            compressedData.resize(compressedSize);
        }
        return result;
    }


    void XmfFile::WriteBufferDescs(StreamWriterLE &pStreamWriter,
                                   std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers) {
        uint32_t dataOffset = 0;
        // TODO kill this
        for (XmfDataBuffer &buffer: buffers) {
            buffer.Description.DataOffset = dataOffset;
            buffer.Description.Compressed = 1;
            buffer.Description.CompressedDataSize = numeric_cast<uint32_t>(compressedBuffers[&buffer].size());
            buffer.Description.Write(pStreamWriter);
            dataOffset += buffer.Description.CompressedDataSize;
        }
    }

    void XmfFile::WriteBuffers(StreamWriterLE &pStreamWriter,
                               std::map<XmfDataBuffer *, std::vector<uint8_t> > &compressedBuffers) {
        for (XmfDataBuffer &buffer: buffers) {
            std::vector<uint8_t> &compressedData = compressedBuffers[&buffer];
            for (int i = 0; i < compressedData.size(); i++) {
                pStreamWriter << compressedData[i];
            }
        }
    }

    aiNode *XmfFile::ConvertToAiNode(const std::string &name, std::shared_ptr<ConversionContext> ctx) {
        auto *pMeshGroupNode = new aiNode();
        pMeshGroupNode->mName = name;
        try {
            if (GetMaterials().empty()) {
                aiMesh *pMesh = ConvertToAiMesh(0, NumIndices(), name, ctx);

                pMeshGroupNode->mMeshes = new uint32_t[1];
                pMeshGroupNode->mMeshes[pMeshGroupNode->mNumMeshes++] = numeric_cast<unsigned int>(ctx->Meshes.size());
                ctx->Meshes.push_back(pMesh);
            } else {
                pMeshGroupNode->mChildren = new aiNode *[NumMaterials()];

                for (XmfMaterial &material : GetMaterials()) {
                    aiMesh *pMesh = ConvertToAiMesh(material.FirstIndex, material.NumIndices,
                                                    name + "X" + replace_all_copy(std::string(material.Name), ".", "X"),
                                                    ctx);

                    auto itMat = ctx->Materials.find(material.Name);
                    if (itMat == ctx->Materials.end()) {
                        pMesh->mMaterialIndex = numeric_cast<unsigned int>(ctx->Materials.size());
                        ctx->Materials[material.Name] = pMesh->mMaterialIndex;
                    } else {
                        pMesh->mMaterialIndex = itMat->second;
                    }

                    auto *pMeshNode = new aiNode();
                    pMeshNode->mName = pMesh->mName;
                    pMeshNode->mMeshes = new uint32_t[1];
                    pMeshNode->mMeshes[pMeshNode->mNumMeshes++] = numeric_cast<unsigned int>(ctx->Meshes.size());
                    ctx->Meshes.push_back(pMesh);

                    pMeshGroupNode->mChildren[pMeshGroupNode->mNumChildren++] = pMeshNode;
                }
            }
        } catch (...) {
            delete pMeshGroupNode;
            throw;
        }
        return pMeshGroupNode;
    }

    aiMesh *XmfFile::ConvertToAiMesh(int firstIndex, int numIndices, const std::string &name,
                                     std::shared_ptr<ConversionContext> ctx) {
        auto *pMesh = new aiMesh();
        pMesh->mName = name;

        try {
            AllocMeshVertices(pMesh, numIndices);
            AllocMeshFaces(pMesh, numIndices);
            PopulateMeshVertices(pMesh, firstIndex, numeric_cast<uint32_t>(numIndices));
            PopulateMeshFaces(pMesh, numIndices);
        } catch (...) {
            delete pMesh;
            throw;
        }
        return pMesh;
    }

    void XmfFile::AllocMeshVertices(aiMesh *pMesh, int numVertices) {
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
                            throw std::runtime_error("Duplicate POSITION vertex element");
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
                                throw std::runtime_error("Duplicate TANGENT vertex element with usage index 0");
                            }
                        } else if (vertexElem.UsageIndex == 1) {
                            if (!pMesh->mBitangents) {
                                pMesh->mBitangents = new aiVector3D[numVertices];
                            } else {
                                throw std::runtime_error("Duplicate TANGENT vertex element with usage index 1");
                            }
                        } else if (vertexElem.UsageIndex > 1) {
                            throw std::runtime_error("Invalid usage index for TANGENT vertex element");
                        }
                        break;

                    case D3DDECLUSAGE_TEXCOORD:
                        if (vertexElem.UsageIndex >= AI_MAX_NUMBER_OF_TEXTURECOORDS) {
                            throw std::runtime_error("Invalid usage index for TEXCOORD vertex element");
                        }
                        if (!pMesh->mTextureCoords[vertexElem.UsageIndex]) {

                            pMesh->mTextureCoords[vertexElem.UsageIndex] = new aiVector3D[numVertices];
                            pMesh->mNumUVComponents[vertexElem.UsageIndex] = 2;
                        } else {
                            throw std::runtime_error("Duplicate TEXCOORD element");
                        }
                        break;

                    case D3DDECLUSAGE_COLOR:
                        if (vertexElem.UsageIndex >= AI_MAX_NUMBER_OF_COLOR_SETS) {
                            throw std::runtime_error("Invalid usage index for COLOR vertex element");
                        }
                        if (!pMesh->mColors[vertexElem.UsageIndex]) {
                            pMesh->mColors[vertexElem.UsageIndex] = new aiColor4D[numVertices];
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

    void XmfFile::AllocMeshFaces(aiMesh *pMesh, int numIndices) {
        if (numIndices <= 0) {
            throw std::runtime_error("AllocMeshFaces: numIndices must be > 0");
        }
        if (numIndices % 3) {
            throw std::runtime_error("AllocMeshFaces: numIndices must be a multiple of 3");
        }
        pMesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
        pMesh->mFaces = new aiFace[numIndices / 3];
    }

    void XmfFile::PopulateMeshVertices(aiMesh *pMesh, int firstIndex, uint32_t numIndices) {
        XmfDataBuffer *pIndexBuffer = GetIndexBuffer();
        if (!pIndexBuffer) {
            throw std::runtime_error("Mesh file has no index buffer");
        }
        if (numIndices <= 0) {
            throw std::runtime_error("PopulateMeshVertices: numIndices must be > 0");
        }
        if (firstIndex < 0) {
            throw std::runtime_error("PopulateMeshVertices: firstIndex must be >= 0");
        }
        if (firstIndex + numIndices > pIndexBuffer->Description.NumItemsPerSection) {
            throw std::runtime_error("PopulateMeshVertices: numIndices is too large");
        }
        uint8_t *pIndexes = pIndexBuffer->GetData();
        D3DFORMAT indexFormat = pIndexBuffer->GetIndexFormat();

        for (XmfDataBuffer &buffer : GetBuffers()) {
            if (!buffer.IsVertexBuffer()) {
                continue;
            }

            uint8_t *pVertexBuffer = buffer.GetData();
            int elemOffset = 0;
            int declarationSize = buffer.Description.ItemSize;

            for (int elemIdx = 0; elemIdx < buffer.Description.NumVertexElements; ++elemIdx) {
                XmfVertexElement &elem = buffer.Description.VertexElements[elemIdx];
                auto elemType = (D3DDECLTYPE) elem.Type;

                for (int vertexIdxIdx = firstIndex; vertexIdxIdx < firstIndex + numIndices; ++vertexIdxIdx) {
                    int vertexIdx = (indexFormat == D3DFMT_INDEX16 ? ((uint16_t *) pIndexes)[vertexIdxIdx]
                                                                   : ((int *) pIndexes)[vertexIdxIdx]);
                    if (vertexIdx < 0 || vertexIdx >= buffer.Description.NumItemsPerSection)
                        throw std::runtime_error("PopulateMeshVertices: invalid index");

                    uint8_t *pVertexElemData = pVertexBuffer + vertexIdx * declarationSize + elemOffset;
                    int localVertexIdx = vertexIdxIdx - firstIndex;
                    switch (elem.Usage) {
                        case D3DDECLUSAGE_POSITION: {
                            aiVector3D position = DXUtil::ConvertVertexAttributeToAiVector3D(pVertexElemData, elemType);
                            position.x = -position.x;
                            pMesh->mVertices[localVertexIdx] = position;
                            break;
                        }

                        case D3DDECLUSAGE_NORMAL: {
                            aiVector3D normal = DXUtil::ConvertVertexAttributeToAiVector3D(pVertexElemData, elemType);
                            normal.x = -normal.x;
                            pMesh->mNormals[localVertexIdx] = normal;
                            break;
                        }

                        case D3DDECLUSAGE_TANGENT: {
                            aiVector3D *pTangents = (elem.UsageIndex == 0 ? pMesh->mTangents : pMesh->mBitangents);
                            aiVector3D tangent = DXUtil::ConvertVertexAttributeToAiVector3D(pVertexElemData, elemType);
                            tangent.x = -tangent.x;
                            pTangents[localVertexIdx] = tangent;
                            break;
                        }

                        case D3DDECLUSAGE_TEXCOORD: {
                            aiVector3D texcoord = DXUtil::ConvertVertexAttributeToAiVector3D(pVertexElemData, elemType);
                            texcoord.y = 1.0f - texcoord.y;
                            pMesh->mTextureCoords[elem.UsageIndex][localVertexIdx] = texcoord;
                            break;
                        }

                        case D3DDECLUSAGE_COLOR: {
                            pMesh->mColors[elem.UsageIndex][localVertexIdx] = DXUtil::ConvertVertexAttributeToColorF(
                                    pVertexElemData, elemType);
                            break;
                        }

                        default:
                            throw std::runtime_error(str(format("Unexpected Usage: %1%") % elem.Usage));
                    }
                }
                elemOffset += util::DXUtil::GetVertexElementTypeSize((D3DDECLTYPE) elem.Type);
            }
        }
        pMesh->mNumVertices = numIndices;
    }

    void XmfFile::PopulateMeshFaces(aiMesh *pMesh, int numIndices) {
        int index = 0;
        for (int i = 0; i < numIndices / 3; ++i) {
            aiFace &face = pMesh->mFaces[pMesh->mNumFaces++];
            face.mIndices = new uint32_t[3];
            while (face.mNumIndices < 3) {
                face.mIndices[face.mNumIndices++] = numeric_cast<unsigned int>(index++);
            }
        }
    }


    std::shared_ptr<XmfFile> XmfFile::GenerateMeshFile(const aiScene *pScene, aiNode *pNode, bool isCollisionMesh) {
        std::vector<aiNode *> meshNodes;
        if (pNode->mNumChildren == 0) {
            meshNodes.push_back(pNode);
        } else {
            for (int i = 0; i < pNode->mNumChildren; ++i) {
                meshNodes.push_back(pNode->mChildren[i]);
            }
        }

        std::shared_ptr<XmfFile> pMeshFile = std::make_shared<XmfFile>();
        pMeshFile->GetBuffers().resize(2);
        XmfDataBuffer &vertexBuffer = pMeshFile->GetBuffers()[0];
        XmfDataBuffer &indexBuffer = pMeshFile->GetBuffers()[1];

        vertexBuffer.Description.Type = 0;
        indexBuffer.Description.Type = 30;

        vertexBuffer.Description.NumSections = 1;
        indexBuffer.Description.NumSections = 1;

        std::vector<XmfVertexElement> vertexDecl;
        for (aiNode *pMeshNode: meshNodes) {
            if (pMeshNode->mNumMeshes == 0) {
                throw std::runtime_error(str(format("Node %s has no mesh attached") % pMeshNode->mName.C_Str()));
            }
            if (pMeshNode->mNumMeshes > 1) {
                throw std::runtime_error(
                        str(format("Node %s has multiple meshes attached") % pMeshNode->mName.C_Str()));
            }
            auto meshIndex = pMeshNode->mMeshes[0];
            aiMesh *pMesh = pScene->mMeshes[meshIndex];
            if (pMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
                throw std::runtime_error(
                        str(format("Mesh %s contains nontriangular polygons") % pMeshNode->mName.C_Str()));
            }
            if (!isCollisionMesh) {
                XmfFile::ExtendVertexDeclaration(pMesh, vertexDecl);
            }
            vertexBuffer.Description.NumItemsPerSection += pMesh->mNumVertices;
            indexBuffer.Description.NumItemsPerSection += pMesh->mNumFaces * 3;
        }
        if (isCollisionMesh) {
            vertexDecl.emplace_back(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION);
        }
        XmfFile::ApplyVertexDeclaration(vertexDecl, vertexBuffer);
        if (vertexBuffer.Description.NumItemsPerSection <= 0xFFFF) {
            indexBuffer.Description.ItemSize = sizeof(uint16_t);
            indexBuffer.Description.Format = 30;
        } else {
            indexBuffer.Description.ItemSize = sizeof(int);
            indexBuffer.Description.Format = 31;
        }

        vertexBuffer.AllocData();
        indexBuffer.AllocData();
        uint8_t *pVertex = vertexBuffer.GetData();
        uint8_t *pIndex = indexBuffer.GetData();
        int vertexOffset = 0;
        int indexOffset = 0;
        for (aiNode *pMeshNode: meshNodes) {
            aiMesh *pMesh = pScene->mMeshes[pMeshNode->mMeshes[0]];
            for (int i = 0; i < pMesh->mNumVertices; ++i) {
                for (XmfVertexElement &vertexElem: vertexDecl) {
                    pVertex += vertexElem.WriteVertexElement(pMesh, i, pVertex);
                }
            }

            for (int i = 0; i < pMesh->mNumFaces; ++i) {
                aiFace *pFace = &pMesh->mFaces[i];
                for (int j = 0; j < 3; ++j) {
                    int index = vertexOffset + pFace->mIndices[j];
                    if (indexBuffer.Description.ItemSize == sizeof(uint16_t)) {
                        *(uint16_t *) pIndex = (uint16_t) index;
                    } else {
                        *(int *) pIndex = index;
                    }
                    pIndex += indexBuffer.Description.ItemSize;
                }
            }

            std::cmatch match;
            if (std::regex_match(pMeshNode->mName.C_Str(), match, std::regex(R"(\w+?-\w+?X(\w+?)X(\w+?))"))) {
                int firstIndex = indexOffset;
                int numIndices = pMesh->mNumFaces * 3;
                const std::string &name = match[1].str() + "." + match[2].str();
                pMeshFile->materials.emplace_back(firstIndex, numIndices, name);
            }
            vertexOffset += pMesh->mNumVertices;
            indexOffset += pMesh->mNumFaces * 3;
        }

        return pMeshFile;
    }

    void XmfFile::ExtendVertexDeclaration(aiMesh *pMesh, std::vector<XmfVertexElement> &declaration) {
        std::vector<XmfVertexElement> meshDeclaration;
        if (pMesh->mVertices)
            meshDeclaration.emplace_back(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION);

        if (pMesh->mNormals)
            meshDeclaration.emplace_back(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_NORMAL);

        if (pMesh->mTangents)
            meshDeclaration.emplace_back(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_TANGENT, 0);

        for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i) {
            if (pMesh->mTextureCoords[i])
                meshDeclaration.emplace_back(D3DDECLTYPE_FLOAT16_2, D3DDECLUSAGE_TEXCOORD, i);
        }

        for (int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i) {
            if (pMesh->mColors[i])
                meshDeclaration.emplace_back(D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, i);
        }

        for (XmfVertexElement &meshElem: meshDeclaration) {
            bool alreadyExists = false;
            for (XmfVertexElement &partElem: declaration) {
                if (meshElem.Usage == partElem.Usage && meshElem.UsageIndex == partElem.UsageIndex) {
                    alreadyExists = true;
                    break;
                }
            }
            if (!alreadyExists)
                declaration.push_back(meshElem);
        }
    }

    // TODO move me to XmfDataBuffer?
    void XmfFile::ApplyVertexDeclaration(std::vector<XmfVertexElement> &declaration, XmfDataBuffer &buffer) {
        if (declaration.size() >
            sizeof(buffer.Description.VertexElements) / sizeof(buffer.Description.VertexElements[0])) {
            throw std::runtime_error("Too many vertex elements in vertex declaration");
        }
        uint32_t declarationSize = 0;

        buffer.Description.NumVertexElements = numeric_cast<uint32_t>(declaration.size());
        for (int i = 0; i < declaration.size(); ++i) {
            buffer.Description.VertexElements[i] = declaration[i];
            declarationSize += DXUtil::GetVertexElementTypeSize((D3DDECLTYPE) declaration[i].Type);
        }
        buffer.Description.ItemSize = declarationSize;
        buffer.Description.DenormalizeVertexDeclaration();
    }


}
