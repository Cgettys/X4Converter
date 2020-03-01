#include <X4ConverterTools/model/MaterialLibrary.h>
#include <boost/numeric/conversion/cast.hpp>
#include <string>
#include <utility>

namespace model {
using namespace boost;
namespace fs=boost::filesystem;
MaterialLibrary::MaterialLibrary(util::FileSystemUtil::Ptr c) : fsUtil(std::move(c)) {

}
Material &MaterialLibrary::GetMaterial(const std::string &dottedName) {
  if (!initialized) {
    init();
    initialized = true;
  }
  std::cmatch match;
  if (!std::regex_match(dottedName.c_str(), match, materialPattern)) {
    throw std::runtime_error("Could not parse material name: " + dottedName);
  }

  auto collIt = collections.find(match[1]);
  if (collIt == collections.end()) {
    throw std::runtime_error("Could not find collection: " + match[1].str());
  }
  return collIt->second.GetMaterial(match[2]);
}
void MaterialLibrary::init() {
  // Being a little bit lazy on when we read the library in (i.e. only if it is used) vastly speeds up tests
  auto filePath = fsUtil->GetAbsolutePath("libraries/material_library.xml");
  if (!fs::is_regular_file(filePath)) {
    throw std::runtime_error(str(format("Failed to load material library: %s does not exist") % filePath));
  }
  pugi::xml_document doc;
  auto result = doc.load_file(filePath.c_str());
  if (result.status != pugi::status_ok) {
    throw std::runtime_error(str(format("Failed to parse material library: %s") % result.description()));
  }
  auto nodeList = doc.select_nodes("/materiallibrary/collection");
  collections.reserve(nodeList.size());
  for (auto x : nodeList) {
    auto collection = x.node();
    std::string collectionName = collection.attribute("name").value();
    if (!collections.try_emplace(collectionName, fsUtil, collection).second) {
      throw std::runtime_error("Duplicated collection: " + collectionName);
    }
  }
}
uint32_t MaterialLibrary::GetIndex(const std::string &matName) {
  auto it = materials.find(matName);
  uint32_t index;
  if (it == materials.end()) {
    index = numeric_cast<unsigned int>(materials.size());
    materials.insert({matName, index});
  } else {
    index = it->second;
  }
  return index;
}
void MaterialLibrary::PopulateScene(aiScene *scene) {
  if (!materials.empty()) {
    std::string modelFolderPath = fs::path(_gameBaseFolderPath).parent_path().string();
    scene->mNumMaterials = numeric_cast<unsigned int>(materials.size());
    scene->mMaterials = new aiMaterial *[scene->mNumMaterials];
    for (auto &it : materials) {
      auto pAiMaterial = GetMaterial(it.first).ConvertToAiMaterial();
      scene->mMaterials[it.second] = pAiMaterial;
    }
  }

}
}