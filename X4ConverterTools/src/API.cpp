#include <X4ConverterTools/API.h>
#include <assimp/DefaultIOSystem.h>

using namespace X4ConverterTools;

bool ConvertXmlToDae(const char *pszGameBaseFolderPath, const char *pszXmlFilePath, const char *pszDaeFilePath) {

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
//    Assimp::Importer* importer = new Assimp::Importer();
//    importer->RegisterLoader(new AssetImporter(pszGameBaseFolderPath));
    AssetImporter importer(pszGameBaseFolderPath);
    aiScene *pScene = new aiScene();// cleaned up by the exporter when it's deleted...
    importer.InternReadFile(pszXmlFilePath, pScene, io);
//    const aiScene *pScene = importer->ReadFile(pszXmlFilePath, 0);
//    if (!pScene) {
//        std::cerr << "Failed during import" << std::endl;
//        strncpy(pszError, importer->GetErrorString(), iMaxErrorSize);
//        delete importer;
//        return false;
//    }


    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(pScene, "collada", pszDaeFilePath);
    if (result != aiReturn_SUCCESS) {
        std::cerr << "Failed during export" << std::endl;
        throw std::runtime_error(exporter.GetErrorString());
    }
    delete io;
    // pScene is cleaned up by the exporter at the moment for better or for worse
    return true;
}

bool ConvertDaeToXml(const char *pszGameBaseFolderPath, const char *pszDaeFilePath, const char *pszXmfFilePath) {
    Assimp::Importer *importer = new Assimp::Importer();
    const aiScene *pScene = importer->ReadFile(pszDaeFilePath, 0);
    if (!pScene) {
        std::cerr << "Failed during import" << std::endl;
        throw std::runtime_error(importer->GetErrorString());
    }


//    Assimp::Exporter* exporter = new Assimp::Exporter();
//    exporter->RegisterExporter(AssetExporter::Format);
//    AssetExporter::GameBaseFolderPath = pszGameBaseFolderPath;
//    aiReturn result = exporter->Export(pScene, "xmf", pszXmfFilePath);
//    if (result != aiReturn_SUCCESS) {
//        std::cerr << "Failed during export" << std::endl;
//        strncpy(pszError, exporter->GetErrorString(), iMaxErrorSize);
//        delete importer;
//        delete exporter;
//        return false;
//    }
//
//    delete importer;
//    delete exporter;
    AssetExporter::GameBaseFolderPath = pszGameBaseFolderPath;

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
    AssetExporter exporter;
    exporter.Export(pszXmfFilePath, io, pScene, nullptr);
    delete io;
    delete importer;
    return true;
}

