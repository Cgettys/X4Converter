#pragma once

class MaterialLibrary;
class MaterialCollection;

class Material
{
public:
                        Material                        ();
                        Material                        ( MaterialCollection* pCollection, pugi::xml_node node );

    MaterialCollection* GetCollection                   () const    { return _pCollection; }

    const std::string&  GetName                         () const    { return _name; }

    std::string         GetDiffuseMapFilePath           () const;
    std::string         GetSpecularMapFilePath          () const;
    std::string         GetNormalMapFilePath            () const;
    std::string         GetEnvironmentMapFilePath       () const;

    aiMaterial*         ConvertToAiMaterial             ( const boost::filesystem::path& modelFolderPath ) const;

private:
    std::string         GetTextureFilePath              ( const std::string& filePath ) const;
    static std::string  GetDecompressedTextureFilePath  ( const std::string& filePath );

    MaterialCollection* _pCollection;
    std::string         _name;
    
    aiColor4D           _emissiveColor;

    std::string         _diffuseMapFilePath;
    std::string         _specularMapFilePath;
    std::string         _normalMapFilePath;
    std::string         _environmentMapFilePath;

    float               _diffuseStrength;
    float               _normalStrength;
    float               _specularStrength;
    float               _environmentStrength;
};
