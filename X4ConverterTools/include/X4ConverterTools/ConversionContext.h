#pragma once

#include <map>
#include <memory>
#include <vector>
#include <assimp/scene.h>

#include <assimp/DefaultIOSystem.h>
#include "X4ConverterTools/model/MaterialLibrary.h"

class ConversionContext {
 public:
  using Ptr = std::shared_ptr<ConversionContext>;
  using path = boost::filesystem::path;
  // TODO move me?
  using MetadataMap = std::map<std::string, std::string>;
  explicit ConversionContext(const std::string &gameBaseFolderPath,
                             const std::string &metadataFilePath,
                             std::shared_ptr<Assimp::IOSystem> io,
                             bool convert,
                             bool is_test);
  ~ConversionContext();

  [[nodiscard]] path GetRelativePath(const path &filePath);
  [[nodiscard]] path GetRelativePath(const path &filePath, const path &relativeToFolderPath);
  [[nodiscard]] path GetAbsolutePath(const path &filePath);

  [[nodiscard]] static std::string MakePlatformSafe(const std::string &filePath);

  [[nodiscard]] static std::string MakeGameSafe(const std::string &filePath);

  [[nodiscard]] static path MakePlatformSafe(const path &filePath);

  [[nodiscard]] static path MakeGameSafe(const path &filePath);

  [[nodiscard]] std::string GetOutputPath(const std::string &inputPath);
  void PopulateSceneArrays();

  void SetSourcePathSuffix(std::string path);

  std::string GetSourcePath();

  Assimp::IOStream *GetSourceFile(const std::string &name, const std::string &mode = "rb");

  std::map<std::string, uint32_t> Materials;
  std::string gameBaseFolderPath;

  void SetScene(aiScene *scene);

  aiMesh *GetMesh(size_t meshIndex);

  bool CheckLight(const std::string &name);
  aiLight *GetLight(const std::string &name);
  std::vector<aiLight *> GetLightsByParent(const std::string &parentName);

  void AddMesh(aiNode *parentNode, aiMesh *mesh);
  void AddLight(aiLight *light);
  // TODO something more elegant?
  // Note, performance heavy
  [[nodiscard]] MetadataMap GetMetadataMap(const std::string &name);

  void AddMetadata(const std::string &name, MetadataMap m);
  // TODO write out


  bool ShouldConvertGeometry() { return should_convert; };

  // TODO somewhere better
  std::regex lodRegex{"[^-]+\\-lod\\d"};
  std::regex collisionRegex{"[^-]+\\-collision"};
 protected:
  bool test;
  bool should_convert;
  std::shared_ptr<Assimp::IOSystem> io;
  aiScene *pScene{};
  std::vector<aiMesh *> meshes;
  std::string sourcePathSuffix;
  model::MaterialLibrary materialLibrary;
  std::map<std::string, aiLight *> lights;
 private:
  const std::string metadataPath;
  std::map<std::string, MetadataMap> allMetadata;
};