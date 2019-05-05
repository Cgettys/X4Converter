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

// Note: This function is crucial. All meshes + materials are stored flat in aiScene, so this does the actual population
// of that array from the vectors that have been built up
void ConversionContext::PopulateSceneArrays() {
    // Add the materials to the scene
    if (!Materials.empty()) {
        std::string modelFolderPath = path(gameBaseFolderPath).parent_path().string();
        pScene->mNumMaterials = numeric_cast<unsigned int>(Materials.size());
        pScene->mMaterials = new aiMaterial *[pScene->mNumMaterials];
        for (auto &it : Materials) {
            Material *pMaterial = materialLibrary.GetMaterial(it.first);
            auto pAiMaterial = pMaterial->ConvertToAiMaterial(modelFolderPath, gameBaseFolderPath);
            pScene->mMaterials[it.second] = pAiMaterial;
        }
    }
    int meshCount = 0;
    // Add the meshes to the scene
    if (!Meshes.empty()) {
        pScene->mNumMeshes = numeric_cast<unsigned int>(Meshes.size());
        pScene->mMeshes = new aiMesh *[pScene->mNumMeshes];
        for (auto &meshIt : Meshes) {
            pScene->mMeshes[meshCount++] = meshIt;
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

