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
#include <X4ConverterTools/util/AssimpUtil.h>

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
    // Add the meshes to the scene
    if (!meshes.empty()) {
        auto meshCount = numeric_cast<unsigned int>(meshes.size());
        pScene->mNumMeshes = meshCount;
        pScene->mMeshes = new aiMesh *[meshCount];

        int meshIdx = 0;
        for (auto &meshIt : meshes) {
            pScene->mMeshes[meshIdx++] = meshIt;
        }
    }
    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        AssimpUtil::MergeVertices(pScene->mMeshes[i]);
    }
    if (!lights.empty()) {
        auto lightCount = numeric_cast<unsigned int>(lights.size());
        pScene->mLights = new aiLight *[lightCount];

        for (auto &lightIt : lights) {
            if (lightIt.second == nullptr) {
                continue;
                // TODO figure out where nullptr ones come from
            }
            pScene->mLights[pScene->mNumLights++] = lightIt.second;
        }
    }
}

void ConversionContext::SetSourcePathSuffix(std::string path) {
    std::cout << "Source path suffix set: " << path << std::endl;
    sourcePathSuffix = std::move(path);
}

std::string ConversionContext::GetSourcePath() {
    if (sourcePathSuffix.empty()) {
        throw std::runtime_error("Source path suffix has not been set!");
    }
    return ConversionContext::MakePlatformSafe(gameBaseFolderPath + "/" + sourcePathSuffix);
}

// callee frees
Assimp::IOStream *ConversionContext::GetSourceFile(const std::string &name, std::string mode) {
    std::string path = ConversionContext::MakePlatformSafe(GetSourcePath() + "/" + algorithm::to_lower_copy(name));
    if (!io->Exists(path) && mode != "wb") {
        throw std::runtime_error("File missing: " + name);
    }

    auto result = io->Open(path, mode);
    if (result == nullptr) {
        if (mode == "wb") {
            throw std::runtime_error("File could not be opened or created: " + name);
        } else {
            throw std::runtime_error("File could not be opened: " + name);
        }
    }
    return result;
}

void ConversionContext::AddLight(aiLight *light) {
    std::string name = light->mName.C_Str();
    if (lights[name]) {
        throw std::runtime_error("Duplicated light name: " + name);
    }
    lights.insert(std::pair<std::string, aiLight *>(name, light));
}

bool ConversionContext::CheckLight(const std::string &name) {
    return lights.count(name) > 0;
}

aiLight *ConversionContext::GetLight(const std::string &name) {
    if (CheckLight(name)) {
        return lights[name];
    }
    throw std::runtime_error("Could not find light of name:" + name);
}

void ConversionContext::SetScene(aiScene *pScene) {
    ConversionContext::pScene = pScene;
    for (int i = 0; i < pScene->mNumLights; i++) {
        ConversionContext::AddLight(pScene->mLights[i]);
    }
    for (int i = 0; i < pScene->mNumMeshes; i++) {
        meshes.emplace_back(pScene->mMeshes[i]);
    }
}

void ConversionContext::AddMesh(aiNode *parentNode, aiMesh *pMesh) {
    if (parentNode->mNumMeshes != 0) {
        throw std::runtime_error("Node had mesh when it should have been empty!");
    }
    parentNode->mNumMeshes = 1;
    parentNode->mMeshes = new uint32_t[1];
    parentNode->mMeshes[0] = numeric_cast<unsigned int>(meshes.size());

    meshes.push_back(pMesh);
}

aiMesh *ConversionContext::GetMesh(int meshIndex) {
    return meshes[meshIndex];
}



