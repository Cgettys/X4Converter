#include <X4ConverterTools/Material.h>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;

Material::Material() {
    _pCollectionName = "";

    _diffuseStrength = NAN;
    _normalStrength = NAN;
    _specularStrength = NAN;
    _environmentStrength = NAN;

}

Material::Material(std::string pCollectionName, pugi::xml_node node) {
    _pCollectionName = pCollectionName;
    _name = node.attribute("name").value();

    pugi::xpath_node_set properties = node.select_nodes("properties/property");
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        std::string type = it->node().attribute("type").value();
        std::string name = it->node().attribute("name").value();
        std::string value = it->node().attribute("value").value();

        if (type == "BitMap") {
            if (name == "diffuse_map")
                _diffuseMapFilePath = value;
            else if (name == "specular_map")
                _specularMapFilePath = value;
            else if (name == "normal_map")
                _normalMapFilePath = value;
            else if (name == "environment_map")
                _environmentMapFilePath = value;
        } else if (type == "Float") {
            float fValue = atof(value.c_str());
            if (name == "diffuseStr")
                _diffuseStrength = fValue;
            else if (name == "normalStr")
                _normalStrength = fValue;
            else if (name == "specularStr")
                _specularStrength = fValue;
            else if (name == "environmentStr")
                _environmentStrength = fValue;
        }
    }
}

aiMaterial *Material::ConvertToAiMaterial(const boost::filesystem::path &modelFolderPath,
                                          const boost::filesystem::path &baseFolderPath) {
    auto pAiMaterial = new aiMaterial();
    std::string nameStr = _pCollectionName + "X" + GetName();
//    aiString *name = new aiString(nameStr);
    aiString name(nameStr);
    // Explicit constructor new aiString is broken
    pAiMaterial->AddProperty(&name, AI_MATKEY_NAME);
//    delete name;
    populateDiffuseLayer(pAiMaterial, modelFolderPath, baseFolderPath);
    populateSpecularLayer(pAiMaterial, modelFolderPath, baseFolderPath);
    populateNormalLayer(pAiMaterial, modelFolderPath, baseFolderPath);
    populateEnvironmentLayer(pAiMaterial, modelFolderPath, baseFolderPath);

//    if (textureFilePath.size() < 1){
//        std::cerr << "Warning, empty textureFilePath for material"<<std::endl;
//    }
//    for (int i = 0; i < pAiMaterial->mNumProperties; i++) {
//        std::string key=pAiMaterial->mProperties[i]->mKey.C_Str();
//        auto value = pAiMaterial->mProperties[i]->mData;
//
//        std::cout<<key <<  value << std::endl;
//    }
//    std::cout << pAiMaterial -> GetName().C_Str() << std::endl;
    return pAiMaterial;
}

void Material::populateDiffuseLayer(aiMaterial *pAiMaterial, const path &modelFolderPath, const path &baseFolderPath) {
    if (!_diffuseMapFilePath.empty()) {
        std::string textureFilePath = GetDecompressedTextureFilePath(_diffuseMapFilePath,baseFolderPath);
        if (!textureFilePath.empty()) {
            aiString temp(PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string());
            pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_DIFFUSE(0));
        } else {
            throw std::runtime_error("Could not find Diffuse Texture for Material!");
        }
    }
}

void Material::populateSpecularLayer(aiMaterial *pAiMaterial, const path &modelFolderPath, const path &baseFolderPath) {
    if (!_specularMapFilePath.empty()) {
        std::string textureFilePath = GetDecompressedTextureFilePath(_specularMapFilePath,baseFolderPath);
        if (!textureFilePath.empty()) {
            aiString temp(PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string());
            pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_SPECULAR(0));
        } else {
            throw std::runtime_error("Could not find Specular Texture for Material!");
        }
    }
}

void Material::populateNormalLayer(aiMaterial *pAiMaterial, const path &modelFolderPath, const path &baseFolderPath) {
    if (!_normalMapFilePath.empty()) {
        std::string textureFilePath = GetDecompressedTextureFilePath(_normalMapFilePath,baseFolderPath);
        if (!textureFilePath.empty()) {
            aiString temp(
                    PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string());
            pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_NORMALS(0));
        } else {
            throw std::runtime_error("Could not find Normal Texture for Material!");
        }
    }
}

void
Material::populateEnvironmentLayer(aiMaterial *pAiMaterial, const path &modelFolderPath, const path &baseFolderPath) {
    if (!_environmentMapFilePath.empty()) {
        std::string textureFilePath = GetDecompressedTextureFilePath(_environmentMapFilePath,baseFolderPath);
        if (!textureFilePath.empty()) {
            aiString temp(PathUtil::GetRelativePath(textureFilePath, modelFolderPath).string());
            pAiMaterial->AddProperty(&temp, AI_MATKEY_TEXTURE_REFLECTION(0));
        } else {
            throw std::runtime_error("Could not find Environment Texture for Material!");
        }
    }
}

const
std::string
Material::GetDecompressedTextureFilePath(const std::string compressedFilePath, const path &baseFolderPath) const {
    std::string filePath = GetTextureFilePath(compressedFilePath, baseFolderPath);
    filePath = PathUtil::MakePlatformSafe(filePath);
    if (filePath.empty()) {
        throw std::runtime_error("Empty path");
    }
    path textureFilePath(filePath);
    if (!iequals(textureFilePath.extension().string(), ".gz")) {
        return textureFilePath.string();
    }

    path uncompressedPath(textureFilePath);
    uncompressedPath.replace_extension(".dds");
    if (is_regular_file(uncompressedPath)) {
        return uncompressedPath.string();
    }
    gzFile pGzFile = gzopen(textureFilePath.string().c_str(), "rb");
    if (!pGzFile) {
        return textureFilePath.string();
    }
    FILE *pDdsFile = fopen(uncompressedPath.string().c_str(), "wb+");
    if (!pDdsFile) {
        gzclose(pGzFile);
        return textureFilePath.string();
    }

    byte buffer[0x400];
    int bytesRead;
    do {
        bytesRead = gzread(pGzFile, buffer, sizeof(buffer));
        fwrite(buffer, 1, bytesRead, pDdsFile);
    } while (bytesRead == sizeof(buffer));

    fclose(pDdsFile);
    gzclose(pGzFile);
    if (uncompressedPath.empty()) {
        std::cerr << "Warning, returned empty uncompressed path (Material.cpp:150)" << std::endl;
    }
    return uncompressedPath.string();
}


const std::string Material::GetTextureFilePath(const std::string tgtFilePath, const path &baseFolderPath) const {
    static const char *ppszAllowedExtensions[] = {"gz", "dds", "tga", "jpg"};

    std::string filePath = PathUtil::MakePlatformSafe(tgtFilePath);

    if (filePath.empty()) {
        throw std::runtime_error("Received empty filepath");
    }
    path textureFilePath(baseFolderPath);
    textureFilePath /= filePath;

    // TODO debug flag   std::cerr << textureFilePath << std::endl;

    if (is_regular_file(textureFilePath)) {
        return textureFilePath.string();
    }
    if (textureFilePath.has_extension()) {
        std::cerr << "Warning textureFilePath has extension" << std::endl;
        return std::string();
    }
    for (int i = 0;
         i < sizeof(ppszAllowedExtensions) / sizeof(ppszAllowedExtensions[0]);
         ++i) {
        textureFilePath.replace_extension(ppszAllowedExtensions[i]);
        if (is_regular_file(textureFilePath)) {
            return textureFilePath.string();
        }
    }
    std::cerr << "Warning returned empty string" << std::endl;
    return std::string();
}