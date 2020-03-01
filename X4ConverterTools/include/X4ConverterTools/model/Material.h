#pragma once

#include <pugixml.hpp>
#include <assimp/scene.h>
#include <X4ConverterTools/util/FileSystemUtil.h>

namespace model {
class Material {
 public:
  Material(util::FileSystemUtil::Ptr c);

  Material(util::FileSystemUtil::Ptr c, std::string pCollectionName, pugi::xml_node &node);

  const std::string &GetName() { return _name; }

  aiMaterial *ConvertToAiMaterial();

 private:
  const util::FileSystemUtil::Ptr fsUtil;

  [[nodiscard]] std::string GetTextureFilePath(const std::string &filePath) const;

  [[nodiscard]] std::string GetDecompressedTextureFilePath(const std::string &filePath) const;

  void PopulateLayer(aiMaterial *pAiMaterial,
                     const std::string &path,
                     const char *key,
                     aiTextureType type,
                     uint32_t num);

  std::string _pCollectionName;
  std::string _name;

  aiColor4D _emissiveColor;

  std::string _diffuseMapFilePath;
  std::string _specularMapFilePath;
  std::string _normalMapFilePath;
  std::string _environmentMapFilePath;

  float _diffuseStrength;
  float _normalStrength;
  float _specularStrength;
  float _environmentStrength;
};
}
