#include <X4ConverterTools/AssetImporter.h>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace Assimp;
using namespace ani;

AssetImporter::AssetImporter(const std::string &gameBaseFolderPath) : context(gameBaseFolderPath) {
    pAnimFile = nullptr;

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
        std::shared_ptr<Component> pComponent = Component::ReadFromFile(filePath, context, pIOHandler);

        // Convert to the Assimp data model
        pScene->mRootNode = pComponent->ConvertToAiNode(context);

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

        context.AddMaterialsToScene(filePath,pScene);

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw DeadlyImportError(e.what());
    }
}



