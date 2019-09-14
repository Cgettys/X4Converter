#pragma once

#include <pugixml.hpp>
#include <boost/filesystem.hpp>
#include <assimp/scene.h>

class ConversionContext;

namespace model {
    class Material {
    public:
        Material();

        Material(std::string pCollectionName, pugi::xml_node node);

        const std::string &GetName() { return _name; }

        aiMaterial *ConvertToAiMaterial(ConversionContext *ctx);

    private:

        const std::string
        GetTextureFilePath(const std::string &filePath, const boost::filesystem::path &baseFolderPath) const;

        const std::string GetDecompressedTextureFilePath(const std::string &filePath,
                                                         const boost::filesystem::path &baseFolderPath) const;

        void
        PopulateLayer(aiMaterial *pAiMaterial, const std::string &path, const char *key, aiTextureType type, int num,
                      ConversionContext *ctx);

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
