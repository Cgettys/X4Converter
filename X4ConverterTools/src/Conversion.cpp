#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/model/Component.h>
#include <X4ConverterTools/ConversionContext.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/DefaultIOSystem.h>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>
#include <pugixml.hpp>

namespace fs = boost::filesystem;

bool
ConvertXmlToDae(const std::string &gameBaseFolderPath, const std::string &xmlFilePath, const std::string &daeFilePath) {
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
    if (load_result.status != pugi::status_ok) {
        throw std::runtime_error("xml file could not be opened!");
    }

    auto io = std::make_shared<Assimp::DefaultIOSystem>();
    auto ctx = std::make_shared<ConversionContext>(gameBaseFolderPath, io);
    model::Component component(doc.root(), ctx);
    aiNode *root = component.ConvertToAiNode(ctx);
    aiScene *pScene = new aiScene();// cleaned up by the exporter when it's deleted...
    pScene->mRootNode = root;

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
    // pScene is cleaned up by the exporter at the moment for better or for worse
    return true;
}

bool
ConvertDaeToXml(const std::string &gameBaseFolderPath, const std::string &daeFilePath, const std::string &xmlFilePath) {
    auto actualDaeFilePath = fs::exists(daeFilePath) ? daeFilePath : gameBaseFolderPath + daeFilePath;
    auto actualXmlFilePath = fs::exists(xmlFilePath) ? xmlFilePath : gameBaseFolderPath + xmlFilePath;
    auto *importer = new Assimp::Importer();
    const aiScene *pScene = importer->ReadFile(actualDaeFilePath, 0);
    if (!pScene) {
        std::cerr << "Failed during import" << std::endl;
        throw std::runtime_error(importer->GetErrorString());
    }

    auto io = std::make_shared<Assimp::DefaultIOSystem>();
    auto ctx = std::make_shared<ConversionContext>(gameBaseFolderPath, io);
    model::Component component(ctx);
    component.ConvertFromAiNode(pScene->mRootNode->mChildren[0], ctx);

    pugi::xml_document doc;
    if (fs::exists(actualXmlFilePath)) {
        auto load_result = doc.load_file(actualXmlFilePath.c_str());
        if (load_result.status != pugi::status_ok) {
            throw std::runtime_error("output xml file could not be opened!");
        }
    } else {
        std::cerr << "Warning, creating output file at path: " << actualXmlFilePath;
    }
    auto tgtNode = doc.child("components");
    if (!tgtNode) {
        tgtNode = doc.append_child("components");
    }
    component.ConvertToGameFormat(tgtNode, ctx);
    doc.save_file(actualXmlFilePath.c_str());

    delete importer;
    return true;
}

