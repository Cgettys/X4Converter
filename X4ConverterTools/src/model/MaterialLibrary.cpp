#include <X4ConverterTools/model/MaterialLibrary.h>

#include <boost/algorithm/string.hpp>
#include <boost/range.hpp>
#include <boost/filesystem.hpp>
#include <assimp/scene.h>

#include <string>
#include <regex>

using namespace boost;
using namespace boost::filesystem;
namespace model {
    MaterialLibrary::MaterialLibrary(const std::string &gameBaseFolderPath) : _gameBaseFolderPath(gameBaseFolderPath) {
        std::string filePath = (path(gameBaseFolderPath) / "libraries" / "material_library.xml").string();
        if (!is_regular_file(filePath)) {
            throw std::runtime_error(str(format("Failed to load material library: %s does not exist") % filePath));
        }
        pugi::xml_document doc;
        auto result = doc.load_file(filePath.c_str());
        if (result.status != pugi::status_ok) {
            throw std::runtime_error(str(format("Failed to parse material library: %s") % result.description()));
        }
        for (auto x : doc.select_nodes("/materiallibrary/collection")) {
            auto collection = x.node();
            std::string collectionName = collection.attribute("name").value();
            collections[collectionName] = MaterialCollection(collection);
        }
    }



    Material *MaterialLibrary::GetMaterial(const std::string &dottedName) {

        std::smatch match;
        std::string regexPattern(R"((\w+)\.(\w+))");
        if (!std::regex_match(dottedName, match, std::regex(regexPattern))) {
            throw std::runtime_error("Could not parse material name: " + dottedName);
        }

        auto collIt = collections.find(match[1]);
        if (collIt == collections.end()) {
            throw std::runtime_error("Could not find collection: " + match[1].str());
        }
        return collIt->second.GetMaterial(match[2]);
    }
}
