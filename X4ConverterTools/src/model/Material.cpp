#include <X4ConverterTools/model/Material.h>

#include <boost/algorithm/string.hpp>
#include <zlib.h>
#include <iostream>
#include <X4ConverterTools/ConversionContext.h>
#include <boost/numeric/conversion/cast.hpp>
#include <cstdint>
#include <utility>

namespace algo= boost::algorithm;
namespace fs = boost::filesystem;
using boost::numeric_cast;
using util::FileSystemUtil;
namespace model {
Material::Material(FileSystemUtil::Ptr c) : fsUtil(std::move(c)) {
  _pCollectionName = "";

  _diffuseStrength = NAN;
  _normalStrength = NAN;
  _specularStrength = NAN;
  _environmentStrength = NAN;

}

Material::Material(FileSystemUtil::Ptr c, std::string pCollectionName, pugi::xml_node &node) : fsUtil(std::move(c)) {
  _pCollectionName = std::move(pCollectionName);
  _name = node.attribute("name").value();

  pugi::xpath_node_set properties = node.select_nodes("properties/property");
  for (auto propertie : properties) {
    std::string type = propertie.node().attribute("type").value();
    std::string name = propertie.node().attribute("name").value();
    std::string value = propertie.node().attribute("value").value();

    if (type == "BitMap") {
      if (name == "diffuse_map") {
        _diffuseMapFilePath = value;
      } else if (name == "specular_map") {
        _specularMapFilePath = value;
      } else if (name == "normal_map") {
        _normalMapFilePath = value;
      } else if (name == "environment_map") {
        _environmentMapFilePath = value;
      }
    } else if (type == "Float") {
      auto strength = numeric_cast<float>(stof(value));
      if (name == "diffuseStr") {
        _diffuseStrength = strength;
      } else if (name == "normalStr") {
        _normalStrength = strength;
      } else if (name == "specularStr") {
        _specularStrength = strength;
      } else if (name == "environmentStr") {
        _environmentStrength = strength;
      }
    }
  }
}

aiMaterial *Material::ConvertToAiMaterial() {
  auto pAiMaterial = new aiMaterial();
  std::string nameStr = _pCollectionName + "X" + GetName();
  const aiString name(nameStr);
  pAiMaterial->AddProperty(&name, AI_MATKEY_NAME);
  PopulateLayer(pAiMaterial, _diffuseMapFilePath, AI_MATKEY_TEXTURE_DIFFUSE(0));
  PopulateLayer(pAiMaterial, _specularMapFilePath, AI_MATKEY_TEXTURE_SPECULAR(0));
  PopulateLayer(pAiMaterial, _normalMapFilePath, AI_MATKEY_TEXTURE_NORMALS(0));
  PopulateLayer(pAiMaterial, _environmentMapFilePath, AI_MATKEY_TEXTURE_REFLECTION(0));
  // TODO what does this do
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

// TODO automate:
// Decompress a second time
// wine '/home/cg/Desktop/X4/DirefsUtilTex/texconv.exe' 'C:\multimat_diff.dds' -o 'C:\multimat_diff_out' -y -f R16G16B16A16_UINT
// Flatten
// wine '/home/cg/Desktop/X4/DirefsUtilTex/texassemble.exe' array-strip 'C:\multimat_diff.dds' -o 'C:\multimat_diff_flat.dds' -f R8G8B8A8_UNORM
void Material::PopulateLayer(aiMaterial *pAiMaterial,
                             const std::string &path,
                             const char *key,
                             aiTextureType type,
                             uint32_t num) {
  if (!path.empty()) {
    // TODO should GetDecompressedTextureFilePath move?
    // TODO how to do the relative path with stuff
    std::string textureFilePath = GetDecompressedTextureFilePath(path);
    if (!textureFilePath.empty()) {
      aiString temp("../../../" +
          fsUtil->GetRelativePath(textureFilePath).string());
      pAiMaterial->AddProperty(&temp, key, type, num);
    } else {
      // throw std::runtime_error("Could not find Diffuse Texture for Material!");
      std::cerr << "WARNING: Could not find Texture for Material!\n";
    }
  }
}

std::string Material::GetTextureFilePath(const std::string &tgtFilePath) const {
  static const char *allowedExtensions[] = {"gz", "dds", "tga", "jpg"};

  std::string filePath = FileSystemUtil::MakePlatformSafe(tgtFilePath);

  if (filePath.empty()) {
    throw std::runtime_error("Received empty filepath");
  }
  fs::path textureFilePath = fsUtil->GetAbsolutePath(filePath);

  // TODO debug flag   std::cerr << textureFilePath << std::endl;

  if (fs::is_regular_file(textureFilePath)) {
    return textureFilePath.string();
  }
  for (auto &allowedExtension : allowedExtensions) {
    textureFilePath.replace_extension(allowedExtension);
    if (is_regular_file(textureFilePath)) {
      return textureFilePath.string();
    }
  }
  if (textureFilePath.has_extension()) {
    std::cerr << "Warning textureFilePath has unexpected extension" << std::endl;
    return std::string();
  }
  std::cerr << "Warning returned empty string" << std::endl;
  return std::string();
}
std::string
Material::GetDecompressedTextureFilePath(const std::string &compressedFilePath) const {
  std::string filePath = GetTextureFilePath(compressedFilePath);
  filePath = FileSystemUtil::MakePlatformSafe(filePath);

  fs::path textureFilePath(filePath);
  if (!algo::iequals(textureFilePath.extension().string(), ".gz")) {
    return textureFilePath.string();
  }

  fs::path uncompressedPath(textureFilePath);
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

  // Appears to decompress compressed textures
  uint8_t buffer[0x400];
  size_t bytesRead;
  do {
    bytesRead = numeric_cast<size_t>(gzread(pGzFile, buffer, sizeof(buffer)));
    fwrite(buffer, 1, bytesRead, pDdsFile);
  } while (bytesRead == sizeof(buffer)); // Until end of file

  fclose(pDdsFile);
  gzclose(pGzFile);
  if (uncompressedPath.empty()) {
    std::cerr << "Warning, returned empty uncompressed path (Material.cpp:150)" << std::endl;
  }
  return uncompressedPath.string();
}

}