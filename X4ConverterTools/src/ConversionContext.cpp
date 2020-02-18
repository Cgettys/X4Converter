#include <X4ConverterTools/ConversionContext.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <utility>
#include <X4ConverterTools/util/AssimpUtil.h>
using namespace boost;
namespace fs = boost::filesystem;

ConversionContext::ConversionContext(const std::string &gameBaseFolderPath,
                                     const std::string &metadataFilePath,
                                     std::shared_ptr<Assimp::IOSystem> io,
                                     bool convert,
                                     bool is_test) :
    materialLibrary(gameBaseFolderPath),
    gameBaseFolderPath(gameBaseFolderPath),
    test(is_test),
    should_convert(convert),
    io(std::move(io)),
    metadataPath(GetAbsolutePath(metadataFilePath).generic_string()) {

}

std::string ConversionContext::MakePlatformSafe(const std::string &filePath) {
  if (filePath.empty()) {
    std::cerr << "Warning, MakePlatformSafe received an empty file path as an argument." << std::endl;
    return filePath;
  }
  std::string result(filePath);

  std::string preferredSep;
  preferredSep.append(1, fs::path::preferred_separator);
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

fs::path ConversionContext::MakePlatformSafe(const fs::path &filePath) {
  return fs::path(MakePlatformSafe(filePath.string()));
}

fs::path ConversionContext::MakeGameSafe(const fs::path &filePath) {
  return fs::path(MakeGameSafe(filePath.string()));
}

fs::path ConversionContext::GetAbsolutePath(const fs::path &filePath) {

  if (!gameBaseFolderPath.empty() && filePath.is_relative()) {
    std::cout << "Prepending " << gameBaseFolderPath << " to path " << filePath << std::endl;
    return gameBaseFolderPath / filePath;
  }
  return filePath.generic_path();
}

fs::path ConversionContext::GetRelativePath(const fs::path &filePath) {
  return GetRelativePath(filePath, gameBaseFolderPath);
}

fs::path ConversionContext::GetRelativePath(const fs::path &filePath, const fs::path &relativeToFolderPath) {
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
  fs::path result;
  for (auto i = differenceStart; i < relativeToFolderPathParts.size(); ++i) {
    result /= "..";
  }

  for (auto i = differenceStart; i < filePathParts.size() - 1; ++i) {
    result /= filePathParts[i];
  }

  result /= filePath.filename();
//    std::cout << "Returned path:"<< filePath << " " << relativeToFolderPath << " "<<  result.string() << std::endl;
  return result;
}

std::string ConversionContext::GetOutputPath(const std::string &inputPath) {
  if (test) {
    std::cout << "Using test paths" << std::endl;
    fs::path p(inputPath);
    std::string ext = ".out";
    ext.append(p.extension().string());
    p.replace_extension(ext);
    return p.generic_string();
  } else {
    return inputPath;
  }

}

using namespace model;

// Note: This function is crucial. All meshes + materials are stored flat in aiScene, so this does the actual population
// of that array from the vectors that have been built up
void ConversionContext::PopulateSceneArrays() {
  if (populatedArrays) {
    throw std::runtime_error("You cannot populate the scene arrays twice!");
  }
  populatedArrays = true;
  // Add the materials to the scene
  if (!Materials.empty()) {
    std::string modelFolderPath = fs::path(gameBaseFolderPath).parent_path().string();
    pScene->mNumMaterials = numeric_cast<unsigned int>(Materials.size());
    pScene->mMaterials = new aiMaterial *[pScene->mNumMaterials];
    for (auto &it : Materials) {
      auto pAiMaterial = materialLibrary.GetMaterial(it.first).ConvertToAiMaterial(this);
      pScene->mMaterials[it.second] = pAiMaterial;
    }
  }
  // Add the meshes to the scene
  if (!meshes.empty()) {
    auto meshCount = numeric_cast<unsigned int>(meshes.size());
    pScene->mNumMeshes = meshCount;
    auto oldMeshes = pScene->mMeshes;
    pScene->mMeshes = new aiMesh *[meshCount];
    delete[] oldMeshes;
    int meshIdx = 0;
    for (auto *meshIt : meshes) {
      pScene->mMeshes[meshIdx] = meshIt;
      ++meshIdx;
    }
  }
  // TODO figure out what this is supposed to do and if necessary fix it
//  for (auto i = 0U; i < pScene->mNumMeshes; ++i) {
//    AssimpUtil::MergeVertices(pScene->mMeshes[i]);
//  }
  if (!lights.empty()) {
    auto lightCount = numeric_cast<unsigned int>(lights.size());
    pScene->mLights = new aiLight *[lightCount];

    for (auto &lightIt : lights) {
      pScene->mLights[pScene->mNumLights++] = &lightIt.second;
    }
  }
  // TODO animation handling here too
  // Reformat it into more readable xml. Yes, JSON might be more natural for this part, but not so much for the animation
  // Also, consistency...
  pugi::xml_document metadoc;
  auto metaroot = metadoc.root();
  auto kvNode = metaroot.child("misc");
  for (const auto &obj : allMetadata) {
    auto objNode = kvNode.child("object");
    objNode.child("name").set_value(obj.first.c_str());
    for (const auto &pair : obj.second) {
      auto attrNode = objNode.child("attribute");
      attrNode.child("key").set_value(pair.first.c_str());
      attrNode.child("value").set_value(pair.second.c_str());
    }
  }
  metadoc.save_file(metadataPath.c_str());
}

void ConversionContext::SetSourcePathSuffix(std::string path) {
  std::cout << "Source path suffix set: " << path << std::endl;
  sourcePathSuffix = std::move(path);
}

std::string ConversionContext::GetSourcePath() {
  if (sourcePathSuffix.empty()) {
    throw std::runtime_error("Source path suffix has not been set!");
  }
  return ConversionContext::MakePlatformSafe(gameBaseFolderPath / sourcePathSuffix).string();
}

Assimp::IOStream *ConversionContext::GetSourceFile(const std::string &name, const std::string &mode) {

  std::string path = ConversionContext::MakePlatformSafe(GetSourcePath() + "/" + algorithm::to_lower_copy(name));
  if (!io->Exists(path) && mode != "wb") {
    throw std::runtime_error("File missing: " + name + " Looked at: " + path);
  }

  Assimp::IOStream *result = io->Open(path, mode);
  if (result == nullptr) {
    std::string human_readable_op;
    if (mode == "wb") {
      throw std::runtime_error("File could not be opened for writing: " + name + " Path used: " + path);
    } else if (mode == "rb") {
      throw std::runtime_error("File could not be opened for reading: " + name + " Path used: " + path);
    } else {
      throw std::runtime_error("File could not be opened for " + mode + ": " + name + " Path used: " + path);
    }
  }
  return result;
}

void ConversionContext::AddLight(aiLight light) {
  std::string name = light.mName.C_Str();
  if (CheckLight(name)) {
    throw std::runtime_error("Duplicated light name: " + name);
  }
  lights[name] = std::move(light);
}

bool ConversionContext::CheckLight(const std::string &name) {
  return lights.count(name) > 0;
}

aiLight &ConversionContext::GetLight(const std::string &name) {
  if (CheckLight(name)) {
    return lights[name];
  }
  throw std::runtime_error("Could not find light of name:" + name);
}

void ConversionContext::SetScene(aiScene *scene) {
  ConversionContext::pScene = scene;
  populatedArrays = false;
  for (auto i = 0U; i < scene->mNumLights; i++) {
    ConversionContext::AddLight(*(scene->mLights[i]));
  }
  for (auto i = 0U; i < scene->mNumMeshes; i++) {
    meshes.emplace_back(scene->mMeshes[i]);
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

aiMesh *ConversionContext::GetMesh(size_t meshIndex) {
  return meshes.at(meshIndex);
}

ConversionContext::MetadataMap &ConversionContext::GetMetadataMap(const std::string &name) {
  if (allMetadata.count(name) == 0) {
    allMetadata[name] = {};
  }
  return allMetadata[name];
}
void ConversionContext::AddMetadata(const std::string &name, MetadataMap m) {
  allMetadata[name] = std::move(m);
}

ConversionContext::~ConversionContext() {
  if (pScene != nullptr && !populatedArrays) {
    // Populate arrays so that the scene can do the dirty work for us.
    ConversionContext::PopulateSceneArrays();
//    delete pScene;

  } else if (pScene == nullptr) {
    // We need to clean up the loose ends
    for (auto mesh: meshes) {
      delete mesh;
    }
  }
}
std::vector<std::reference_wrapper<aiLight>> ConversionContext::GetLightsByParent(const std::string &name) {
  std::vector<std::reference_wrapper<aiLight>> matches{};
  const auto key = name + "|lights|";
  auto it = lights.lower_bound(key);
  while (it != lights.end() && it->first.find(key) != std::string::npos) {
    matches.emplace_back(it->second);
    it = std::next(it);
  }
  return matches;
}
