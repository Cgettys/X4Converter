#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/model/Component.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <pugixml.hpp>
namespace fs = boost::filesystem;

bool
ConvertXmlToDae(const ConversionContext::Ptr &ctx,
                const std::string &xmlFilePath,
                const std::string &daeFilePath) {

  pugi::xml_document doc;
  pugi::xml_parse_result load_result;

  // TODO better way to do extension and path handling / generate a Config object to ease integration testing.
  bool relative_paths = false;
  if (fs::exists(xmlFilePath)) {
    load_result = doc.load_file((xmlFilePath).c_str());
  } else if (fs::exists(ctx->gameBaseFolderPath + xmlFilePath)) {
    std::cout << "Paths appear to be relative" << std::endl;
    load_result = doc.load_file((ctx->gameBaseFolderPath + xmlFilePath).c_str());
    relative_paths = true;
  }
  if (load_result.status != pugi::status_ok) {
    throw std::runtime_error("xml file could not be opened!");
  }
  auto *pScene = new aiScene();// cleaned up by the exporter when it's deleted...
  ctx->SetScene(pScene);
  auto root_xml = doc.root();
  model::Component component(root_xml, ctx);
  aiNode *root = component.ConvertToAiNode();
  pScene->mRootNode = root;
  ctx->PopulateSceneArrays();

//    fs::path aniPath(daeFilePath);
//    aniPath.replace_extension("anixml");
//    pugi::xml_document animdoc;
//    pugi::xml_node rt = animdoc.root().append_child("root");

  auto actualDaeFilePath = relative_paths ? ctx->gameBaseFolderPath + daeFilePath : daeFilePath;
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

bool
ConvertDaeToXml(const ConversionContext::Ptr &ctx,
                const std::string &rawDaeFilePath,
                const std::string &rawXmlFilePath) {
  // TODO make better

  auto daeFilePath = ctx->GetAbsolutePath(rawDaeFilePath).generic_string();
  auto xmlFilePath = ctx->GetAbsolutePath(rawXmlFilePath).generic_string();
  auto importer = std::make_unique<Assimp::Importer>();
  importer->SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_USE_COLLADA_NAMES, 1);
  const aiScene *pScene = importer->ReadFile(daeFilePath, 0);
  if (!pScene) {
    std::cerr << "Failed during import" << std::endl;
    throw std::runtime_error(importer->GetErrorString());
  }
  aiScene *myScene;
  aiCopyScene(pScene, &myScene);
  ctx->SetScene(myScene);
  model::Component component(ctx);

  component.ConvertFromAiNode(myScene->mRootNode->mChildren[0]);

  pugi::xml_document doc;
  if (fs::exists(xmlFilePath)) {
    auto load_result = doc.load_file(xmlFilePath.c_str());
    if (load_result.status != pugi::status_ok) {
      throw std::runtime_error("output xml file could not be opened!");
    }
  } else {
    std::cerr << "Warning, creating output file at path: " << xmlFilePath;
  }
  auto tgtNode = doc.child("components");
  if (!tgtNode) {
    tgtNode = doc.append_child("components");
  }
  component.ConvertToGameFormat(tgtNode);
  doc.save_file(xmlFilePath.c_str());

  return true;
}

