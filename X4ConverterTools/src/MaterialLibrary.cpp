#include <X4ConverterTools/MaterialLibrary.h>

using namespace boost;
using namespace boost::filesystem;

MaterialLibrary::MaterialLibrary(const std::string &gameBaseFolderPath) : _gameBaseFolderPath(gameBaseFolderPath) {
    std::string filePath = (path(gameBaseFolderPath) / "libraries" / "material_library.xml").string();
    if (!is_regular_file(filePath)) {
        throw std::runtime_error(str(format("Failed to load material library: %s does not exist") % filePath));
    }
    auto result = _doc.load_file(filePath.c_str());
    if (result.status != pugi::status_ok) {
        throw std::runtime_error(str(format("Failed to parse material library: %s") % result.description()));
    }
}

MaterialCollection *MaterialLibrary::GetCollection(const std::string &name) {
    auto it = _collections.find(name);
    if (it != _collections.end()) {
        return &(it->second);
    }

    pugi::xml_node collectionNode = _doc.select_node(
            (format("/materiallibrary/collection[@name='%1%']") % name.c_str()).str().c_str()).node();
    if (!collectionNode) {
        return nullptr;
    }

    _collections[name] = MaterialCollection(collectionNode);
    return &_collections[name];
}

Material *MaterialLibrary::GetMaterial(const std::string &dottedName) {
    std::smatch match;
    std::string regexPattern(R"((\w+)\.(\w+))");
    if (!std::regex_match(dottedName, match, std::regex(regexPattern))) {
        return nullptr;
    }
    auto pCollection = GetCollection(match[1]);
    if (!pCollection) {
        return nullptr;
    }
    return pCollection->GetMaterial(match[2]);
}
