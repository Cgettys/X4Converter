#include <X4ConverterTools/Xmf/XmfExporter.h>

using namespace boost;

Assimp::Exporter::ExportFormatEntry XmfExporter::Format("xmf", "EgoSoft XuMeshFile exporter", ".xml",
                                                        XmfExporter::Export);
std::string XmfExporter::GameBaseFolderPath;

void XmfExporter::Export(const char *pFilePath, Assimp::IOSystem *pIOHandler, const aiScene *pScene,
                         const Assimp::ExportProperties *props) {
    try {
        if (GameBaseFolderPath.empty()) {
            throw std::runtime_error("GameBaseFolderPath not set");
        }
        if (!pScene->mRootNode || pScene->mRootNode->mNumChildren != 1) {
            throw std::runtime_error(
                    "Scene must have exactly one root node (make sure to remove any lights and cameras)");
        }
        for (int i = 0; i < pScene->mNumMeshes; ++i) {
            AssimpUtil::MergeVertices(pScene->mMeshes[i]);
        }

        aiNode *pComponentNode = pScene->mRootNode->mChildren[0];

        Component component;
        component.Name = pComponentNode->mName.C_Str();
        for (int i = 0; i < pComponentNode->mNumChildren; ++i) {
            ConvertPartNode(component, "", pScene, pComponentNode->mChildren[i]);
        }

        component.WriteToFile(pFilePath, GameBaseFolderPath, pIOHandler);
    }
    catch (std::exception &e) {
        throw DeadlyExportError(e.what());
    }
}

void XmfExporter::ConvertPartNode(Component &component, const std::string &parentPartName, const aiScene *pScene,
                                  aiNode *pPartNode) {
    std::string partName = pPartNode->mName.C_Str();
    if (!std::regex_match(partName, std::regex("\\w+")))
        throw std::runtime_error(
                str(format("Invalid part name %s: must contain only letters, numbers and underscores") % partName));

    if (component.Parts.find(partName) != component.Parts.end())
        throw std::runtime_error(str(format("Duplicate part name %s") % partName));

    ComponentPart &part = component.Parts[partName];
    part.Name = partName;
    part.ParentName = parentPartName;
    part.Position = aiVector3D(
            -pPartNode->mTransformation.a4,
            pPartNode->mTransformation.b4,
            pPartNode->mTransformation.c4
    );

    for (int i = 0; i < pPartNode->mNumChildren; ++i) {
        aiNode *pChildNode = pPartNode->mChildren[i];
        std::string childName = std::string(pChildNode->mName.C_Str());
        std::match_results<std::string::const_iterator> childNameMatch;

        if (std::regex_match(childName, childNameMatch, std::regex(partName + "Xlod(\\d+)"))) {
            int lodIndex = atoi(childNameMatch[1].str().c_str());
            ConvertLodNode(part, lodIndex, pScene, pChildNode);
        } else if (childName == partName + "Xcollision") {
            ConvertCollisionNode(part, pScene, pChildNode);
            CalculatePartSize(part, pScene, pChildNode);
        } else if (std::regex_match(childName, std::regex("\\w+"))) {
            ConvertPartNode(component, partName, pScene, pChildNode);
        } else {
            throw std::runtime_error((format("Cannot determine node type for node %s") % childName).str());
        }
    }
}

void XmfExporter::ConvertCollisionNode(ComponentPart &part, const aiScene *pScene, aiNode *pCollisionNode) {
    part.CollisionMesh = ConvertMeshNode(pScene, pCollisionNode, true);
}

void XmfExporter::ConvertLodNode(ComponentPart &part, int lodIndex, const aiScene *pScene, aiNode *pLodNode) {
    for (ComponentPartLod &lod: part.Lods) {
        if (lod.LodIndex == lodIndex)
            throw std::runtime_error((format("Duplicate lod index %d for part %s") % lodIndex % part.Name).str());
    }

    part.Lods.emplace_back(lodIndex, ConvertMeshNode(pScene, pLodNode, false));
}

std::shared_ptr<XuMeshFile> XmfExporter::ConvertMeshNode(const aiScene *pScene, aiNode *pNode, bool isCollisionMesh) {
    std::vector<aiNode *> meshNodes;
    if (pNode->mNumChildren == 0) {
        meshNodes.push_back(pNode);
    } else {
        for (int i = 0; i < pNode->mNumChildren; ++i) {
            meshNodes.push_back(pNode->mChildren[i]);
        }
    }

    std::shared_ptr<XuMeshFile> pMeshFile = std::make_shared<XuMeshFile>();
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
            throw std::runtime_error(
                    str(format("Node %s has no mesh attached") % pMeshNode->mName.C_Str()));
        }
        if (pMeshNode->mNumMeshes > 1) {
            throw std::runtime_error(
                    str(format("Node %s has multiple meshes attached") % pMeshNode->mName.C_Str()));
        }
        aiMesh *pMesh = pScene->mMeshes[pMeshNode->mMeshes[0]];
        if (pMesh->mPrimitiveTypes & ~aiPrimitiveType_TRIANGLE) {
            throw std::runtime_error(str(format("Mesh %s contains nontriangular polygons") % pMeshNode->mName.C_Str()));
        }
        if (!isCollisionMesh) {
            ExtendVertexDeclaration(pMesh, vertexDecl);
        }
        vertexBuffer.Description.NumItemsPerSection += pMesh->mNumVertices;
        indexBuffer.Description.NumItemsPerSection += pMesh->mNumFaces * 3;
    }
    if (isCollisionMesh) {
        vertexDecl.emplace_back(D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION);
    }
    ApplyVertexDeclaration(vertexDecl, vertexBuffer);
    if (vertexBuffer.Description.NumItemsPerSection <= 0xFFFF) {
        indexBuffer.Description.ItemSize = sizeof(ushort);
        indexBuffer.Description.Format = 30;
    } else {
        indexBuffer.Description.ItemSize = sizeof(int);
        indexBuffer.Description.Format = 31;
    }

    vertexBuffer.AllocData();
    indexBuffer.AllocData();
    byte *pVertex = vertexBuffer.GetData();
    byte *pIndex = indexBuffer.GetData();
    int vertexOffset = 0;
    int indexOffset = 0;
    for (aiNode *pMeshNode: meshNodes) {
        aiMesh *pMesh = pScene->mMeshes[pMeshNode->mMeshes[0]];
        for (int i = 0; i < pMesh->mNumVertices; ++i) {
            for (XmfVertexElement &vertexElem: vertexDecl) {
                pVertex += WriteVertexElement(pMesh, i, vertexElem, pVertex);
            }
        }

        for (int i = 0; i < pMesh->mNumFaces; ++i) {
            aiFace *pFace = &pMesh->mFaces[i];
            for (int j = 0; j < 3; ++j) {
                int index = vertexOffset + pFace->mIndices[j];
                if (indexBuffer.Description.ItemSize == sizeof(ushort)) {
                    *(ushort *) pIndex = (ushort) index;
                } else {
                    *(int *) pIndex = index;
                }
                pIndex += indexBuffer.Description.ItemSize;
            }
        }

        std::cmatch match;
        if (std::regex_match(pMeshNode->mName.C_Str(), match, std::regex(R"(\w+?X\w+?X(\w+?)X(\w+?))"))) {
            pMeshFile->AddMaterial(indexOffset, pMesh->mNumFaces * 3, match[1].str() + "." + match[2].str());
        }
        vertexOffset += pMesh->mNumVertices;
        indexOffset += pMesh->mNumFaces * 3;
    }

    return pMeshFile;
}

void XmfExporter::CalculatePartSize(ComponentPart &part, const aiScene *pScene, aiNode *pCollisionNode) {
    if (pCollisionNode->mNumMeshes == 0)
        return;

    aiVector3D lowerBound;
    aiVector3D upperBound;
    aiMesh *pCollisionMesh = pScene->mMeshes[pCollisionNode->mMeshes[0]];
    for (int i = 0; i < pCollisionMesh->mNumVertices; ++i) {
        aiVector3D &position = pCollisionMesh->mVertices[i];
        if (i == 0) {
            lowerBound = position;
            upperBound = position;
        } else {
            lowerBound.x = std::min(lowerBound.x, position.x);
            lowerBound.y = std::min(lowerBound.y, position.y);
            lowerBound.z = std::min(lowerBound.z, position.z);

            upperBound.x = std::max(upperBound.x, position.x);
            upperBound.y = std::max(upperBound.y, position.y);
            upperBound.z = std::max(upperBound.z, position.z);
        }
    }

    part.Size = aiVector3D(
            (upperBound.x - lowerBound.x) / 2.0f,
            (upperBound.y - lowerBound.y) / 2.0f,
            (upperBound.z - lowerBound.z) / 2.0f
    );
    part.Center = aiVector3D(
            lowerBound.x + part.Size.x,
            lowerBound.y + part.Size.y,
            lowerBound.z + part.Size.z
    );
}

void XmfExporter::ExtendVertexDeclaration(aiMesh *pMesh, std::vector<XmfVertexElement> &declaration) {
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

void XmfExporter::ApplyVertexDeclaration(std::vector<XmfVertexElement> &declaration, XmfDataBuffer &buffer) {
    if (declaration.size() > sizeof(buffer.Description.VertexElements) / sizeof(buffer.Description.VertexElements[0])) {
        throw std::runtime_error("Too many vertex elements in vertex declaration");
    }
    int declarationSize = 0;

    buffer.Description.NumVertexElements = declaration.size();
    for (int i = 0; i < declaration.size(); ++i) {
        buffer.Description.VertexElements[i] = declaration[i];
        declarationSize += DXUtil::GetVertexElementTypeSize((D3DDECLTYPE) declaration[i].Type);
    }
    buffer.Description.ItemSize = declarationSize;
    buffer.Description.DenormalizeVertexDeclaration();
}

int XmfExporter::WriteVertexElement(aiMesh *pMesh, int vertexIdx, XmfVertexElement &elem, byte *pElemData) {
    auto type = (D3DDECLTYPE) elem.Type;
    switch (elem.Usage) {
        case D3DDECLUSAGE_POSITION: {
            aiVector3D position;
            if (pMesh->mVertices) {
                position = pMesh->mVertices[vertexIdx];
                position.x = -position.x;
            }

            return DXUtil::WriteVec3DToVertexAttribute(position, type, pElemData);
        }

        case D3DDECLUSAGE_NORMAL: {
            aiVector3D normal;
            if (pMesh->mNormals) {
                normal = pMesh->mNormals[vertexIdx];
                normal.x = -normal.x;
            }

            return DXUtil::WriteVec3DToVertexAttribute(normal, type, pElemData);
        }

        case D3DDECLUSAGE_TANGENT: {
            aiVector3D tangent;
            if (pMesh->mTangents) {
                tangent = pMesh->mTangents[vertexIdx];
                tangent.x = -tangent.x;
            }

            return DXUtil::WriteVec3DToVertexAttribute(tangent, type, pElemData);
        }

        case D3DDECLUSAGE_TEXCOORD: {
            aiVector3D texcoord;
            if (pMesh->mTextureCoords[elem.UsageIndex]) {
                texcoord = pMesh->mTextureCoords[elem.UsageIndex][vertexIdx];
                texcoord.y = 1.0f - texcoord.y;
            }

            return DXUtil::WriteVec3DToVertexAttribute(texcoord, type, pElemData);
        }

        case D3DDECLUSAGE_COLOR: {
            aiColor4D color;
            if (pMesh->mColors[elem.UsageIndex])
                color = pMesh->mColors[elem.UsageIndex][vertexIdx];

            return DXUtil::WriteColorFToVertexAttribute(color, type, pElemData);
        }
        default:
            throw std::runtime_error("Usage not recognized");
    }
}
