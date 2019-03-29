#include <X4ConverterTools/API.h>
#include <assimp/DefaultIOSystem.h>
using namespace xmf;

bool ConvertXmlToDae(const char *pszGameBaseFolderPath,
                     const char *pszXmlFilePath, const char *pszDaeFilePath, char *pszError,
                     int iMaxErrorSize) {

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
//    Assimp::Importer* importer = new Assimp::Importer();
//    importer->RegisterLoader(new XmfImporter(pszGameBaseFolderPath));
    XmfImporter importer(pszGameBaseFolderPath);
    aiScene* pScene = new aiScene();// cleaned up by the exporter when it's deleted...
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
        strncpy(pszError, exporter.GetErrorString(), iMaxErrorSize);
//        delete importer;
        return false;
    }
    delete io;
//    delete pScene;
    return true;
}

bool ConvertDaeToXml(const char *pszGameBaseFolderPath,
                     const char *pszDaeFilePath, const char *pszXmfFilePath, char *pszError,
                     int iMaxErrorSize) {
    Assimp::Importer* importer = new Assimp::Importer();
    const aiScene *pScene = importer->ReadFile(pszDaeFilePath, 0);
    if (!pScene) {
        std::cerr << "Failed during import" << std::endl;
        strncpy(pszError, importer->GetErrorString(), iMaxErrorSize);
        delete importer;
        return false;
    }


//    Assimp::Exporter* exporter = new Assimp::Exporter();
//    exporter->RegisterExporter(XmfExporter::Format);
//    XmfExporter::GameBaseFolderPath = pszGameBaseFolderPath;
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
    XmfExporter::GameBaseFolderPath = pszGameBaseFolderPath;

    Assimp::IOSystem *io = new Assimp::DefaultIOSystem();
    XmfExporter exporter;
    exporter.Export(pszXmfFilePath, io, pScene, nullptr);
    delete io;
    delete importer;
    return true;
}

