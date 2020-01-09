#include <X4ConverterTools/model/MaterialLibrary.h>

#include <boost/range.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <utility>

using namespace boost;
using namespace boost::filesystem;
namespace model {
MaterialLibrary::MaterialLibrary(std::string gameBaseFolderPath) : _gameBaseFolderPath(std::move(gameBaseFolderPath)) {

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
  std::string filePath = (path(_gameBaseFolderPath) / "libraries" / "material_library.xml").string();
  if (!is_regular_file(filePath)) {
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
    if (!collections.try_emplace(collectionName, collection).second) {
      throw std::runtime_error("Duplicated collection: " + collectionName);
    }
  }

}
}
