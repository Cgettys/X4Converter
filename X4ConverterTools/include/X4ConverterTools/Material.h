#pragma once

#include "pugixml.hpp"
#include "zlib.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <assimp/scene.h>
#include <X4ConverterTools/Util/PathUtil.h>

class Material {
public:
    Material();

    Material(std::string pCollectionName, pugi::xml_node node);

    std::string GetCollectionName() const { return _pCollectionName; }

    const std::string &GetName() const { return _name; }


    aiMaterial *ConvertToAiMaterial(const boost::filesystem::path &modelFolderPath) const;

private:
    std::string GetTextureFilePath(std::string filePath, const boost::filesystem::path &modelFolderPath) const;

    std::string
    GetDecompressedTextureFilePath(std::string filePath, const boost::filesystem::path &modelFolderPath) const;

    void populateDiffuseLayer(aiMaterial *pAiMaterial, const boost::filesystem::path &modelFolderPath) const;

    void populateSpecularLayer(aiMaterial *pAiMaterial, const boost::filesystem::path &modelFolderPath) const;

    void populateNormalLayer(aiMaterial *pAiMaterial, const boost::filesystem::path &modelFolderPath) const;

    void populateEnvironmentLayer(aiMaterial *pAiMaterial, const boost::filesystem::path &modelFolderPath) const;

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
