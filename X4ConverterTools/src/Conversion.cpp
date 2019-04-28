#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/model/Component.h>

namespace fs = boost::filesystem;

bool
ConvertXmlToDae(const std::string &gameBaseFolderPath, const std::string &xmlFilePath, const std::string &daeFilePath) {

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
    pugi::xml_document doc;
    pugi::xml_parse_result load_result;
    bool relative_paths = false;
    if (fs::exists(xmlFilePath)) {
        load_result = doc.load_file((xmlFilePath).c_str());
    } else if (fs::exists(gameBaseFolderPath + xmlFilePath)) {
        std::cout << "Paths appear to be relative" << std::endl;
        load_result = doc.load_file((gameBaseFolderPath + xmlFilePath).c_str());
        relative_paths = true;
    }
    if(load_result.status!=pugi::status_ok){
        throw std::runtime_error("xml file could not be opened!");
    }
    model::Component component(doc.root());
    aiNode* root = component.ConvertToAiNode();
    aiScene *pScene = new aiScene();// cleaned up by the exporter when it's deleted...
    pScene->mRootNode=root;

//    fs::path aniPath(daeFilePath);
//    aniPath.replace_extension("anixml");
//    pugi::xml_document animdoc;
//    pugi::xml_node rt = animdoc.root().append_child("root");

    auto actualDaeFilePath = relative_paths ? gameBaseFolderPath + daeFilePath : daeFilePath;
    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(pScene, "collada", actualDaeFilePath);
    if (result != aiReturn_SUCCESS) {
        std::cerr << "Failed during export" << std::endl;
        throw std::runtime_error(exporter.GetErrorString());
    }


    delete pScene;
    delete io;
    // pScene is cleaned up by the exporter at the moment for better or for worse
    return true;
}

bool
ConvertDaeToXml(const std::string &gameBaseFolderPath, const std::string &daeFilePath, const std::string &xmfFilePath) {
    auto actualDaeFilePath = fs::exists(daeFilePath) ? daeFilePath : gameBaseFolderPath + daeFilePath;
    auto *importer = new Assimp::Importer();
    const aiScene *pScene = importer->ReadFile(actualDaeFilePath, 0);
    if (!pScene) {
        std::cerr << "Failed during import" << std::endl;
        throw std::runtime_error(importer->GetErrorString());
    }

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();


    delete io;
    delete importer;
    return true;
}

