#include <X4ConverterTools/Xmf/XmfImporter.h>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;

XmfImporter::XmfImporter(const std::string &gameBaseFolderPath) :
        _materialLibrary(gameBaseFolderPath) {
    _gameBaseFolderPath = gameBaseFolderPath;
}

const aiImporterDesc *XmfImporter::GetInfo() const {
    static aiImporterDesc info;
    if (!info.mAuthor) {
        info.mName = "EgoSoft XuMeshFile importer";
        info.mAuthor = "arc_";
        info.mFileExtensions = "xml";
        info.mMinMajor = 3;
        info.mMinMinor = 0;
        info.mMaxMajor = 3;
        info.mMaxMinor = 0;
    }
    return &info;
}

bool XmfImporter::CanRead(const std::string &filePath, IOSystem *pIOHandler,
                          bool checkSig) const {
    return iends_with(filePath, ".xml");
}

void XmfImporter::InternReadFile(const std::string &filePath, aiScene *pScene,
                                 IOSystem *pIOHandler) {
    try {
        // Read the .xml and .xmf files
        std::shared_ptr<Component> pComponent = Component::ReadFromFile(
                filePath, _gameBaseFolderPath, pIOHandler);

        // Convert to the Assimp data model
        ConversionContext context;
        pScene->mRootNode = ConvertComponentToAiNode(*pComponent, context);

        // Add the meshes to the scene
        pScene->mMeshes = new aiMesh *[context.Meshes.size()];
        for (aiMesh *pMesh : context.Meshes) {
            AssimpUtil::MergeVertices(pMesh);
            pScene->mMeshes[pScene->mNumMeshes++] = pMesh;
        }

        // Add the materials to the scene
        if (!context.Materials.empty()) {
            std::string baseFolderPath = path(filePath).parent_path().string();
            pScene->mNumMaterials = context.Materials.size();
            pScene->mMaterials = new aiMaterial *[pScene->mNumMaterials];
            for (auto &it : context.Materials) {
                Material *pMaterial = _materialLibrary.GetMaterial(it.first);
                aiMaterial *pAiMaterial;
                if (pMaterial) {
                    pAiMaterial = pMaterial->ConvertToAiMaterial(
                            baseFolderPath);
                } else {
                    auto *tempString = new aiString(it.first);
                    pAiMaterial = new aiMaterial();
                    pAiMaterial->AddProperty(tempString, AI_MATKEY_NAME);
                    delete tempString;
                }
                pScene->mMaterials[it.second] = pAiMaterial;
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw DeadlyImportError(e.what());
    }
}

aiNode *XmfImporter::ConvertComponentToAiNode(Component &component,
                                              ConversionContext &context) {
    std::map<std::string, aiNode *> partNodes;

    // Create nodes and meshes
    for (auto &Part : component.Parts) {
        partNodes[Part.first] = ConvertComponentPartToAiNode(Part.second,
                                                             context);
    }

    // Link parent nodes
    std::vector<aiNode *> rootNodes;
    std::map<aiNode *, std::vector<aiNode *> > nodeChildren;
    for (auto &Part : component.Parts) {
        aiNode *pPartNode = partNodes[Part.first];
        if (Part.second.ParentName.empty()) {
            rootNodes.push_back(pPartNode);
            continue;
        }

        auto parentIt = partNodes.find(Part.second.ParentName);
        if (parentIt == partNodes.end()) {
            throw std::runtime_error(
                    str(
                            format("Node %1% has invalid parent %2%") % (Part.first).c_str()
                            % (Part.second.ParentName).c_str()));
        }
        nodeChildren[parentIt->second].push_back(pPartNode);
    }

    for (auto &it : nodeChildren) {
        aiNode *pParentNode = it.first;
        auto **ppNewChildren = new aiNode *[pParentNode->mNumChildren
                                            + it.second.size()];
        if (pParentNode->mChildren) {
            memcpy(ppNewChildren, pParentNode->mChildren,
                   sizeof(aiNode *) * pParentNode->mNumChildren);
            delete[] pParentNode->mChildren;
        }
        pParentNode->mChildren = ppNewChildren;

        for (aiNode *pChildNode : it.second) {
            pParentNode->mChildren[pParentNode->mNumChildren++] = pChildNode;
            pChildNode->mParent = pParentNode;
        }
    }

// Create component node that contains all the part nodes
    if (rootNodes.empty())
        throw std::runtime_error("No root parts found");

    auto *pComponentNode = new aiNode(component.Name);
    pComponentNode->mChildren = new aiNode *[rootNodes.size()];

    aiNode *pRealRootNode = new aiNode("ROOT_NODE_PLZ");
    pRealRootNode->mChildren = new aiNode *[1];
    pRealRootNode->mChildren[0] = pComponentNode;
    pRealRootNode->mNumChildren = 1;
    pComponentNode->mParent = pRealRootNode;
    for (int i = 0; i < rootNodes.size(); i++) {
        aiNode *pRootNode = rootNodes[i];
        pComponentNode->mChildren[i] = pRootNode;
        pRootNode->mParent = pComponentNode;
        pComponentNode->mNumChildren++;
    }
    return pRealRootNode;
//    return pComponentNode;
}

aiNode *XmfImporter::ConvertComponentPartToAiNode(ComponentPart &part,
                                                  ConversionContext &context) {
    auto *pPartNode = new aiNode();
    try {
        pPartNode->mName = part.Name;
        pPartNode->mTransformation.a4 = -part.Position.x;
        pPartNode->mTransformation.b4 = part.Position.y;
        pPartNode->mTransformation.c4 = part.Position.z;

        pPartNode->mChildren = new aiNode *[part.Lods.size()
                                            + (part.CollisionMesh ? 1 : 0)];

        for (ComponentPartLod &lod : part.Lods) {
            pPartNode->mChildren[pPartNode->mNumChildren++] =
                    ConvertXuMeshToAiNode(*lod.Mesh,
                                          (format("%sXlod%d") % part.Name % lod.LodIndex).str(),
                                          context);
        }
        if (part.CollisionMesh)
            pPartNode->mChildren[pPartNode->mNumChildren++] =
                    ConvertXuMeshToAiNode(*part.CollisionMesh,
                                          part.Name + "Xcollision", context);
    } catch (...) {
        delete pPartNode;
        throw;
    }
    return pPartNode;
}

aiNode *XmfImporter::ConvertXuMeshToAiNode(XuMeshFile &mesh,
                                           const std::string &name, ConversionContext &context) {
    auto *pMeshGroupNode = new aiNode();
    pMeshGroupNode->mName = name;
    try {
        if (mesh.GetMaterials().empty()) {
            aiMesh *pMesh = ConvertXuMeshToAiMesh(mesh, 0, mesh.NumIndices(),
                                                  name, context);

            pMeshGroupNode->mMeshes = new uint[1];
            pMeshGroupNode->mMeshes[pMeshGroupNode->mNumMeshes++] =
                    context.Meshes.size();
            context.Meshes.push_back(pMesh);
        } else {
            pMeshGroupNode->mChildren = new aiNode *[mesh.NumMaterials()];

            for (XmfMaterial &material : mesh.GetMaterials()) {
                aiMesh *pMesh = ConvertXuMeshToAiMesh(mesh, material.FirstIndex,
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
                pMeshNode->mMeshes = new uint[1];
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

aiMesh *XmfImporter::ConvertXuMeshToAiMesh(XuMeshFile &mesh, int firstIndex,
                                           int numIndices, const std::string &name, ConversionContext &context) {
    auto *pMesh = new aiMesh();
    pMesh->mName = name;
    try {
        AllocMeshVertices(pMesh, mesh, numIndices);
        AllocMeshFaces(pMesh, mesh, numIndices);
        PopulateMeshVertices(pMesh, mesh, firstIndex, numIndices);
        PopulateMeshFaces(pMesh, numIndices);
    } catch (...) {
        delete pMesh;
        throw;
    }
    return pMesh;
}

void XmfImporter::AllocMeshVertices(aiMesh *pMesh, XuMeshFile &meshFile,
                                    int numVertices) {
    if (numVertices <= 0) {
        throw std::runtime_error("AllocMeshVertices: numVertices must be > 0");
    }

    for (XmfDataBuffer &buffer : meshFile.GetBuffers()) {
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

void XmfImporter::AllocMeshFaces(aiMesh *pMesh, XuMeshFile &meshFile,
                                 int numIndices) {
    if (numIndices <= 0) {
        throw std::runtime_error("AllocMeshFaces: numIndices must be > 0");
    }
    if (numIndices % 3) {
        throw std::runtime_error(
                "AllocMeshFaces: numIndices must be a multiple of 3");
    }
    pMesh->mFaces = new aiFace[numIndices / 3];
}

void XmfImporter::PopulateMeshVertices(aiMesh *pMesh, XuMeshFile &meshFile,
                                       int firstIndex, int numIndices) {
    XmfDataBuffer *pIndexBuffer = meshFile.GetIndexBuffer();
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

    for (XmfDataBuffer &buffer : meshFile.GetBuffers()) {
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
                        ((ushort *) pIndexes)[vertexIdxIdx] :
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
                        aiVector3D position = DXUtil::ConvertVertexAttributeToVec3D(
                                pVertexElemData, elemType);
                        position.x = -position.x;
                        pMesh->mVertices[localVertexIdx] = position;
                        break;
                    }

                    case D3DDECLUSAGE_NORMAL: {
                        aiVector3D normal = DXUtil::ConvertVertexAttributeToVec3D(
                                pVertexElemData, elemType);
                        normal.x = -normal.x;
                        pMesh->mNormals[localVertexIdx] = normal;
                        break;
                    }

                    case D3DDECLUSAGE_TANGENT: {
                        aiVector3D *pTangents = (
                                elem.UsageIndex == 0 ?
                                pMesh->mTangents : pMesh->mBitangents);
                        aiVector3D tangent = DXUtil::ConvertVertexAttributeToVec3D(
                                pVertexElemData, elemType);
                        tangent.x = -tangent.x;
                        pTangents[localVertexIdx] = tangent;
                        break;
                    }

                    case D3DDECLUSAGE_TEXCOORD: {
                        aiVector3D texcoord = DXUtil::ConvertVertexAttributeToVec3D(
                                pVertexElemData, elemType);
                        texcoord.y = 1.0f - texcoord.y;
                        pMesh->mTextureCoords[elem.UsageIndex][localVertexIdx] =
                                texcoord;
                        break;
                    }

                    case D3DDECLUSAGE_COLOR: {
                        pMesh->mColors[elem.UsageIndex][localVertexIdx] =
                                DXUtil::ConvertVertexAttributeToColorF(
                                        pVertexElemData, elemType);
                        break;
                    }

                    default:
                        throw std::runtime_error(str(format("Unexpected Usage: %1%") % elem.Usage));
                }
            }
            elemOffset += DXUtil::GetVertexElementTypeSize(
                    (D3DDECLTYPE) elem.Type);
        }
    }
    pMesh->mNumVertices = numIndices;
}

void XmfImporter::PopulateMeshFaces(aiMesh *pMesh, int numIndices) {
    int index = 0;
    for (int i = 0; i < numIndices / 3; ++i) {
        aiFace &face = pMesh->mFaces[pMesh->mNumFaces++];
        face.mIndices = new uint[3];
        while (face.mNumIndices < 3) {
            face.mIndices[face.mNumIndices++] = index++;
        }
    }
}
