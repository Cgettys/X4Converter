#include <X4ConverterTools/ConversionContext.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>
#include <X4ConverterTools/util/PathUtil.h>

using namespace boost;
using namespace boost::filesystem;

ConversionContext::ConversionContext(const std::string &gameBaseFolderPath, std::shared_ptr<Assimp::IOSystem> io)
        : materialLibrary(gameBaseFolderPath), gameBaseFolderPath(gameBaseFolderPath), io(io) {

}

using namespace model;

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

void ConversionContext::SetSourcePathSuffix(std::string path) {
    sourcePathSuffix = path;
}

std::string ConversionContext::GetSourcePath() {
    if (sourcePathSuffix.empty()) {
        throw std::runtime_error("Source path suffix has not been set!");
    }
    return PathUtil::MakePlatformSafe(gameBaseFolderPath + "/" + sourcePathSuffix);
}

// callee frees
Assimp::IOStream *ConversionContext::GetSourceFile(std::string name, std::string mode) {
    std::string path = PathUtil::MakePlatformSafe(GetSourcePath() + "/" + name);
    if (!io->Exists(path)) {
        throw std::runtime_error("Source file missing: " + name);
    }
    auto result = io->Open(path, mode);
    if (result == nullptr) {
        throw std::runtime_error("Source file could not be opened: " + name);
    }
    return result;
}

