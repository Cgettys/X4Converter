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
  aiScene *pScene = new aiScene();// cleaned up by the exporter when it's deleted...
  ctx->SetScene(pScene);
  model::Component component(doc.root(), ctx);
  aiNode *root = component.ConvertToAiNode(pugi::xml_node());
  pScene->mRootNode = root;
  ctx->PopulateSceneArrays();

//    fs::path aniPath(daeFilePath);
//    aniPath.replace_extension("anixml");
//    pugi::xml_document animdoc;
//    pugi::xml_node rt = animdoc.root().append_child("root");

  auto actualDaeFilePath = relative_paths ? gameBaseFolderPath + daeFilePath : daeFilePath;
  Assimp::Exporter exporter;
//    Assimp::ExportProperties props;
//    props.SetPropertyBool("COLLADA_EXPORT_USE_MESH_NAMES",true);
  aiReturn result = exporter.Export(pScene, "collada", actualDaeFilePath);//, 0, &props);
  if (result != aiReturn_SUCCESS) {
    std::cerr << "Failed during export" << std::endl;
    throw std::runtime_error(exporter.GetErrorString());
  }
  return true;
}

std::string
PrependIfNecessary(const std::string &gameBaseFolderPath, const std::string &filePath, const std::string &ext) {
  if (!gameBaseFolderPath.empty() && filePath.find(gameBaseFolderPath) == std::string::npos) {
    std::cout << "Prepending " << gameBaseFolderPath << " to path " << filePath << std::endl;
    return gameBaseFolderPath + filePath;
  }

  std::cout << "Using " << ext << " at " << filePath << std::endl;
  return filePath;
}

bool
ConvertDaeToXml(const std::string &gameBaseFolderPath, const std::string &daeFilePath, const std::string &xmlFilePath) {
  std::string actualDaeFilePath = PrependIfNecessary(gameBaseFolderPath, daeFilePath, ".dae");
  std::string actualXmlFilePath = PrependIfNecessary(gameBaseFolderPath, xmlFilePath, ".xml");
  auto importer = std::make_unique<Assimp::Importer>();
  importer->SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_USE_COLLADA_NAMES, 1);
  const aiScene *pScene = importer->ReadFile(actualDaeFilePath, 0);
  if (!pScene) {
    std::cerr << "Failed during import" << std::endl;
    throw std::runtime_error(importer->GetErrorString());
  }
  aiScene *myScene;
  aiCopyScene(pScene, &myScene);
  auto io = std::make_shared<Assimp::DefaultIOSystem>();
  auto ctx = std::make_shared<ConversionContext>(gameBaseFolderPath, io);
  ctx->SetScene(myScene);
  model::Component component(ctx);

  component.ConvertFromAiNode(myScene->mRootNode->mChildren[0], pugi::xml_node());

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
  component.ConvertToGameFormat(tgtNode);
  doc.save_file(actualXmlFilePath.c_str());

  return true;
}

