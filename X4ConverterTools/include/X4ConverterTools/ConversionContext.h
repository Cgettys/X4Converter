#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <assimp/scene.h>

#include <assimp/DefaultIOSystem.h>
#include <X4ConverterTools/util/FileSystemUtil.h>
#include "X4ConverterTools/model/MaterialLibrary.h"
#include "X4ConverterTools/util/MetadataStore.h"

class ConversionContext {
 public:
  using Ptr = std::shared_ptr<ConversionContext>;
  ConversionContext(const util::FileSystemUtil::Ptr &fsUtil,
                    const std::string &metadataFilePath,
                    bool convert);
  ~ConversionContext();
  void PopulateSceneArrays();

  void SetScene(aiScene *scene);

  aiMesh *GetMesh(size_t meshIndex);

  void AddMesh(aiNode *parentNode, aiMesh *mesh);
  [[nodiscard]] uint32_t GetMaterialIndex(const std::string &matName) {
    return materialLibrary.GetIndex(matName);
  }

  bool ShouldConvertGeometry() { return should_convert; };

  // TODO somewhere better
  std::regex lodRegex{"[^-]+\\-lod\\d"};
  std::regex collisionRegex{"[^-]+\\-collision"};
  util::MetadataStore::Ptr metadata;
  util::FileSystemUtil::Ptr fsUtil;
 protected:
  bool should_convert;
  bool populatedArrays = false;
  std::optional<aiScene *> pScene;
  std::vector<aiMesh *> meshes;
  model::MaterialLibrary materialLibrary;
  std::map<std::string, aiLight> lights;
};