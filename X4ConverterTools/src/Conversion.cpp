#include <X4ConverterTools/Conversion.h>

namespace fs = boost::filesystem;

bool
ConvertXmlToDae(const std::string &gameBaseFolderPath, const std::string &xmlFilePath, const std::string &daeFilePath) {

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
//    Assimp::Importer* importer = new Assimp::Importer();
//    importer->RegisterLoader(new AssetImporter(GameBaseFolderPath));
    AssetImporter importer(gameBaseFolderPath);
    aiScene *pScene = new aiScene();// cleaned up by the exporter when it's deleted...

    importer.InternReadFile(xmlFilePath, pScene, io);
//    const aiScene *pScene = importer->ReadFile(XmlFilePath, 0);
//    if (!pScene) {
//        std::cerr << "Failed during import" << std::endl;
//        strncpy(Error, importer->GetErrorString(), iMaxErrorSize);
//        delete importer;
//        return false;
//    }

    fs::path aniPath(daeFilePath);
    aniPath.replace_extension("anixml");
    pugi::xml_document doc;
    pugi::xml_node rt = doc.root().append_child("root");
    std::cout << importer.pAnimFile->validate();
    importer.pAnimFile->WriteAnims(xmlFilePath, rt);
    doc.save_file(aniPath.c_str());
//    ani::AnimFile animFile;
//    pScene->mMetaData->Get("AnimFile", animFile);

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(pScene, "collada", daeFilePath);
    if (result != aiReturn_SUCCESS) {
        std::cerr << "Failed during export" << std::endl;
        throw std::runtime_error(exporter.GetErrorString());
    }


    delete io;
    // pScene is cleaned up by the exporter at the moment for better or for worse
    return true;
}

bool
ConvertDaeToXml(const std::string &gameBaseFolderPath, const std::string &daeFilePath, const std::string &xmfFilePath) {
    Assimp::Importer *importer = new Assimp::Importer();
    const aiScene *pScene = importer->ReadFile(daeFilePath, 0);
    if (!pScene) {
        std::cerr << "Failed during import" << std::endl;
        throw std::runtime_error(importer->GetErrorString());
    }
//    pScene->mMetaData->Add("GameBaseFolderPath", gameBaseFolderPath);


//    Assimp::Exporter* exporter = new Assimp::Exporter();
//    exporter->RegisterExporter(AssetExporter::Format);
//    aiReturn result = exporter->Export(pScene, "xmf", XmfFilePath);
//    if (result != aiReturn_SUCCESS) {
//        std::cerr << "Failed during export" << std::endl;
//        strncpy(Error, exporter->GetErrorString(), iMaxErrorSize);
//        delete importer;
//        delete exporter;
//        return false;
//    }
//
//    delete importer;
//    delete exporter;

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
    AssetExporter exporter;
    AssetExporter::gameBaseFolderPath = gameBaseFolderPath;

    exporter.Export(xmfFilePath.c_str(), io, pScene, nullptr);
    delete io;
    delete importer;
    return true;
}

