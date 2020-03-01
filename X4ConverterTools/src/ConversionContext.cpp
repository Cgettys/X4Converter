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

ConversionContext::ConversionContext(const util::FileSystemUtil::Ptr &fsUtil,
                                     const std::string &metadataFilePath,
                                     bool convert) :
    materialLibrary(fsUtil),
    fsUtil(fsUtil),
    metadata(std::make_unique<util::MetadataStore>(metadataFilePath)),
    should_convert(convert) {
}


// Note: This function is crucial. All meshes + materials are stored flat in aiScene, so this does the actual population
// of that array from the vectors that have been built up
void ConversionContext::PopulateSceneArrays() {
  if (populatedArrays) {
    throw std::runtime_error("You cannot populate the scene arrays twice!");
  }
  if (!pScene.has_value()) {
    throw std::runtime_error("Cannot populate the scene arrays when the scene has not been set!");
  }
  populatedArrays = true;
  auto scene = pScene.value();
  // Add the materials to the scene
  materialLibrary.PopulateScene(scene);
  // Add the meshes to the scene
  if (!meshes.empty()) {
    auto meshCount = numeric_cast<unsigned int>(meshes.size());
    scene->mNumMeshes = meshCount;
    auto oldMeshes = scene->mMeshes;
    scene->mMeshes = new aiMesh *[meshCount];
    delete[] oldMeshes;
    int meshIdx = 0;
    for (auto *meshIt : meshes) {
      scene->mMeshes[meshIdx] = meshIt;
      ++meshIdx;
    }
  }
  // TODO figure out what this is supposed to do and if necessary fix it
//  for (auto i = 0U; i < scene->mNumMeshes; ++i) {
//    AssimpUtil::MergeVertices(scene->mMeshes[i]);
//  }
  if (!lights.empty()) {
    auto lightCount = numeric_cast<unsigned int>(lights.size());
    scene->mLights = new aiLight *[lightCount];

    for (auto &lightIt : lights) {
      scene->mLights[scene->mNumLights++] = &lightIt.second;
    }
  }
  // TODO animation handling here too
  // Reformat it into more readable xml. Yes, JSON might be more natural for this part, but not so much for the animation
  // Also, consistency...
  metadata->WriteDocument();
}


void ConversionContext::SetScene(aiScene *scene) {
  ConversionContext::pScene = scene;
  populatedArrays = false;
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

ConversionContext::~ConversionContext() {
  if (pScene.has_value()) {
    if (!populatedArrays) {
      // Populate arrays so that the scene can do the dirty work for us.
      ConversionContext::PopulateSceneArrays();
    }
  } else {
    // We need to clean up the loose ends
    for (auto mesh: meshes) {
      delete mesh;
    }
  }
}
