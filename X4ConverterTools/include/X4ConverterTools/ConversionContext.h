#pragma once

#include <map>
#include <memory>
#include <vector>
#include <assimp/scene.h>
#include <regex.h>

#include <assimp/DefaultIOSystem.h>
#include "X4ConverterTools/model/MaterialLibrary.h"

class ConversionContext {
 public:
  using Ptr = std::shared_ptr<ConversionContext>;
  using MetadataMap = std::map<std::string, std::string>;
  explicit ConversionContext(const std::string &gameBaseFolderPath,
                             std::shared_ptr<Assimp::IOSystem> io,
                             bool convert,
                             bool is_test);
  ~ConversionContext();
  static boost::filesystem::path
  GetRelativePath(const boost::filesystem::path &filePath, const boost::filesystem::path &relativeToFolderPath);

  static std::string MakePlatformSafe(const std::string &filePath);

  static std::string MakeGameSafe(const std::string &filePath);

  static boost::filesystem::path MakePlatformSafe(const boost::filesystem::path &filePath);

  static boost::filesystem::path MakeGameSafe(const boost::filesystem::path &filePath);

  std::string GetOutputPath(const std::string &inputPath);
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

  void AddMesh(aiNode *parentNode, aiMesh *mesh);
  void AddLight(aiLight *light);
  // TODO something more elegant?
  // Note, performance heavy
  MetadataMap GetMetadataMap(const std::string &name);

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
  std::map<std::string, MetadataMap> allMetadata;
};