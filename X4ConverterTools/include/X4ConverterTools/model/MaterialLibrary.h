#pragma once

#include <string>
#include <regex>
#include <X4ConverterTools/util/FileSystemUtil.h>
#include <X4ConverterTools/model/Material.h>
#include <X4ConverterTools/model/MaterialCollection.h>
#include <X4ConverterTools/types.h>

namespace model {
class MaterialLibrary {
  friend Material;

 public:
  explicit MaterialLibrary(util::FileSystemUtil::Ptr c);

  Material &GetMaterial(const std::string &dottedName);

  // Get the index of the material, adding it to the index if necessary
  uint32_t GetIndex(const std::string &matName);
  // Populate the materials into the scene
  void PopulateScene(aiScene *scene);
 private:
  void init();
  const util::FileSystemUtil::Ptr fsUtil;
  // Note lack of thread safety
  bool initialized = false;
  std::regex materialPattern{R"((\w+)\.(\w+))"};
  std::string _gameBaseFolderPath;
  std::unordered_map<std::string, MaterialCollection> collections;

  // Materials currently in use and their indices
  std::map<std::string, uint32_t> materials;
};
}