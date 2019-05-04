#include <X4ConverterTools/ConversionContext.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
using namespace boost;
using namespace boost::filesystem;

ConversionContext::ConversionContext(const std::string &gameBaseFolderPath) : materialLibrary(gameBaseFolderPath),
                                                                              gameBaseFolderPath(gameBaseFolderPath) {

}

void ConversionContext::AddMaterialsToScene(const std::string &filePath, aiScene *pScene) {
    // Add the materials to the scene
    if (!Materials.empty()) {
        std::string modelFolderPath = path(filePath).parent_path().string();
        pScene->mNumMaterials = numeric_cast<unsigned int>(Materials.size());
        pScene->mMaterials = new aiMaterial *[pScene->mNumMaterials];
        for (auto &it : Materials) {
            Material *pMaterial = materialLibrary.GetMaterial(it.first);
            aiMaterial *pAiMaterial;
            if (pMaterial) {
                pAiMaterial = pMaterial->ConvertToAiMaterial(modelFolderPath, gameBaseFolderPath);
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

