#include <X4ConverterTools/AssetImporter.h>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;
using namespace ani;

AssetImporter::AssetImporter(const std::string &gameBaseFolderPath) : _materialLibrary(gameBaseFolderPath) {
    _gameBaseFolderPath = gameBaseFolderPath;
}

const aiImporterDesc *AssetImporter::GetInfo() const {
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

bool AssetImporter::CanRead(const std::string &filePath, IOSystem *pIOHandler, bool checkSig) const {
    return iends_with(filePath, ".xml");
}

void AssetImporter::InternReadFile(const std::string &filePath, aiScene *pScene, IOSystem *pIOHandler) {
    try {
        // Read the .xml and .xmf files
        std::shared_ptr<Component> pComponent = Component::ReadFromFile(filePath, _gameBaseFolderPath, pIOHandler);

        // Convert to the Assimp data model
        ConversionContext context;
        pScene->mRootNode = ConvertComponentToAiNode(*pComponent, context);

        // Add the meshes to the scene
        pScene->mMeshes = new aiMesh *[context.Meshes.size()];
        for (aiMesh *pMesh : context.Meshes) {
            AssimpUtil::MergeVertices(pMesh);
            pScene->mMeshes[pScene->mNumMeshes++] = pMesh;
        }

        // TODO
//        // ANI file stuff
        // TODO more robust
        std::string shortName = path(filePath).filename().replace_extension("").string() + "_data.ani";
        to_upper(shortName);
        std::string aniPath = (path(filePath).parent_path() / shortName).string();
        IOStream *pAniStream = pIOHandler->Open(aniPath, "rb");
        if (pAniStream == nullptr) {
            std::cerr << "No ANI file found at path: " << aniPath << ". This likely indicates an error." << std::endl;
        } else {
            pAnimFile = new AnimFile(pAniStream);
//            // So we can get it back on the other end
//            pScene->mMetaData=aiMetadata::Alloc(1);
//            pScene->mMetaData->Add("AnimFile",pAnimFile);
        }
//        pScene->mNumAnimations = aniFile.getHeader().getNumAnims();
//
        AddMaterials(filePath, pScene, context);

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw DeadlyImportError(e.what());
    }
}

void AssetImporter::AddMaterials(const std::string &filePath, aiScene *pScene, const ConversionContext &context) {
    // Add the materials to the scene
    if (!context.Materials.empty()) {
        std::string modelFolderPath = path(filePath).parent_path().string();
        pScene->mNumMaterials = numeric_cast<uint>(context.Materials.size());
        pScene->mMaterials = new aiMaterial *[pScene->mNumMaterials];
        for (auto &it : context.Materials) {
            Material *pMaterial = _materialLibrary.GetMaterial(it.first);
            aiMaterial *pAiMaterial;
            if (pMaterial) {
                pAiMaterial = pMaterial->ConvertToAiMaterial(modelFolderPath, _gameBaseFolderPath);
            } else {
                std::cerr << "Warning, weird case" << std::endl;
                auto *tempString = new aiString(it.first);
                pAiMaterial = new aiMaterial();
                pAiMaterial->AddProperty(tempString, AI_MATKEY_NAME);
                delete tempString;
            }
            pScene->mMaterials[it.second] = pAiMaterial;
        }
    }
}

aiNode *AssetImporter::ConvertComponentToAiNode(Component &component, ConversionContext &context) {
    std::map<std::string, aiNode *> partNodes;

    // Create nodes and meshes
    for (auto &Part : component.Parts) {
        partNodes[Part.first] = ConvertComponentPartToAiNode(Part.second, context);
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
            throw std::runtime_error(str(format("Node %1% has invalid parent %2%") % (Part.first).c_str() %
                                         (Part.second.ParentName).c_str()));
        }
        nodeChildren[parentIt->second].push_back(pPartNode);
    }

    for (auto &it : nodeChildren) {
        aiNode *pParentNode = it.first;
        auto **ppNewChildren = new aiNode *[pParentNode->mNumChildren + it.second.size()];
        if (pParentNode->mChildren) {
            memcpy(ppNewChildren, pParentNode->mChildren, sizeof(aiNode *) * pParentNode->mNumChildren);
            delete[] pParentNode->mChildren;
        }
        pParentNode->mChildren = ppNewChildren;

        for (aiNode *pChildNode : it.second) {
            pParentNode->mChildren[pParentNode->mNumChildren++] = pChildNode;
            pChildNode->mParent = pParentNode;
        }
    }

    // Create component node that contains all the part nodes
    if (rootNodes.empty()) {
        throw std::runtime_error("No root parts found");
    }
    auto *pComponentNode = new aiNode(component.Name);
    pComponentNode->mChildren = new aiNode *[rootNodes.size()];

    aiNode *pRealRootNode = new aiNode("Scene");// To make blender happy name root node Scene?
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

aiNode *AssetImporter::ConvertComponentPartToAiNode(ComponentPart &part, ConversionContext &context) {
    auto *pPartNode = new aiNode();
    try {
        pPartNode->mName = part.Name;

        // TODO push this into part
        auto outRot  = part.Rot;// * aiQuaternion(0,-M_PI,0);
        // -X because handedness I guess. But we have to do the same when applying other data
        (pPartNode->mTransformation) = aiMatrix4x4(aiVector3D(1, 1, 1), outRot,
                                                   aiVector3D(-part.Position.x, part.Position.y, part.Position.z));

        pPartNode->mChildren = new aiNode *[part.Lods.size() + (part.CollisionMesh ? 1 : 0)];

        for (ComponentPartLod &lod : part.Lods) {
            const std::string name = (format("%sXlod%d") % part.Name % lod.LodIndex).str();
            pPartNode->mChildren[pPartNode->mNumChildren++] = lod.Mesh->ConvertToAiNode(name, context);
        }
        if (part.CollisionMesh) {
            std::string name = part.Name + "Xcollision";
            pPartNode->mChildren[pPartNode->mNumChildren++] = part.CollisionMesh->ConvertToAiNode(name, context);
        }
    } catch (...) {
        // TODO real exception handling
        delete pPartNode;
        throw;
    }
    return pPartNode;
}

