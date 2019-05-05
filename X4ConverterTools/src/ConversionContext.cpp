#include <X4ConverterTools/ConversionContext.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>
#include <X4ConverterTools/ConversionContext.h>

using namespace boost;
using namespace boost::filesystem;

ConversionContext::ConversionContext(const std::string &gameBaseFolderPath, std::shared_ptr<Assimp::IOSystem> io)
        : materialLibrary(gameBaseFolderPath), gameBaseFolderPath(gameBaseFolderPath), io(io) {

}

#include <X4ConverterTools/ConversionContext.h>

#include <string>
#include <iostream>
#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <X4ConverterTools/types.h>

using namespace boost;
using namespace boost::filesystem;

std::string ConversionContext::MakePlatformSafe(const std::string &filePath) {
    if (filePath.empty()) {
        std::cerr << "Warning, MakePlatformSafe received an empty file path as an argument." << std::endl;
        return filePath;
    }
    std::string result(filePath);

    std::string preferredSep;
    preferredSep.append(1, path::preferred_separator);
    //TODO check for C://?
    algorithm::replace_all(result, "\\", preferredSep);
    algorithm::replace_all(result, "/", preferredSep);
    if (result.empty()) {
        throw std::runtime_error("Empty path after path made platform safe");
    }
    return result;
}

std::string ConversionContext::MakeGameSafe(const std::string &filePath) {
    std::string result(filePath);
    algorithm::replace_all(result, "//", "\\");
    return result;
}

path ConversionContext::MakePlatformSafe(const path &filePath) {
    return path(MakePlatformSafe(filePath.string()));
}

path ConversionContext::MakeGameSafe(const path &filePath) {
    return path(MakeGameSafe(filePath.string()));
}

path ConversionContext::GetRelativePath(const path &filePath, const path &relativeToFolderPath) {
    if (filePath.is_relative()) {
        throw std::runtime_error("filePath is already relative");
    }
    if (!relativeToFolderPath.is_absolute()) {
        throw std::runtime_error("relativeToPath is not absolute");
    }
    std::vector<std::string> filePathParts;

    split(filePathParts, filePath.string(), [](char c) { return c == '/' || c == '\\'; });


    std::vector<std::string> relativeToFolderPathParts;
    split(relativeToFolderPathParts, relativeToFolderPath.string(), [](char c) { return c == '/' || c == '\\'; });

    //https://stackoverflow.com/a/17270869
    filePathParts.erase(std::remove(filePathParts.begin(), filePathParts.end(), ""), filePathParts.end());

    relativeToFolderPathParts.erase(std::remove(relativeToFolderPathParts.begin(), relativeToFolderPathParts.end(), ""),
                                    relativeToFolderPathParts.end());

    int differenceStart = 0;
    while (differenceStart < filePathParts.size() - 1 && differenceStart < relativeToFolderPathParts.size() &&
           iequals(filePathParts[differenceStart], relativeToFolderPathParts[differenceStart])) {
        differenceStart++;
    }

    if (differenceStart == 0) {
        std::cerr << "Paths lack common root" << std::endl;
        throw std::runtime_error(str(format("Paths do not have a common root %1% %2%") % filePath.c_str() %
                                     relativeToFolderPath.c_str()));
    }
    path result;
    for (int i = differenceStart; i < relativeToFolderPathParts.size(); ++i) {
        result /= "..";
    }

    for (int i = differenceStart; i < filePathParts.size() - 1; ++i) {
        result /= filePathParts[i];
    }

    result /= filePath.filename();
//    std::cout << "Returned path:"<< filePath << " " << relativeToFolderPath << " "<<  result.string() << std::endl;
    return result;
}


std::string ConversionContext::GetOutputPath(const std::string &inputPath) {

#ifdef X4ConverterTools_TEST
    std::cout << "Using test paths" << std::endl;
    path p(inputPath);
    std::string ext = ".out";
    ext.append(p.extension().string());
    p.replace_extension(ext);
    return p.generic_string();
#else
    return inputPath;
#endif
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
            auto pAiMaterial = pMaterial->ConvertToAiMaterial(this);
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
    sourcePathSuffix = std::move(path);
}

std::string ConversionContext::GetSourcePath() {
    if (sourcePathSuffix.empty()) {
        throw std::runtime_error("Source path suffix has not been set!");
    }
    return ConversionContext::MakePlatformSafe(gameBaseFolderPath + "/" + sourcePathSuffix);
}

// callee frees
Assimp::IOStream *ConversionContext::GetSourceFile(std::string name, std::string mode) {
    std::string path = ConversionContext::MakePlatformSafe(GetSourcePath() + "/" + name);
    if (!io->Exists(path)) {
        throw std::runtime_error("Source file missing: " + name);
    }
    auto result = io->Open(path, mode);
    if (result == nullptr) {
        throw std::runtime_error("Source file could not be opened: " + name);
    }
    return result;
}

