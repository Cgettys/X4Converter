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


    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw DeadlyImportError(e.what());
    }
}



