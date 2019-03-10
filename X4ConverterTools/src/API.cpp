#include <X4ConverterTools/API.h>

bool ConvertXmlToDae(const char* pszGameBaseFolderPath,
		const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError,
		int iMaxErrorSize) {
	Assimp::Importer importer;
	importer.RegisterLoader(new XmfImporter(pszGameBaseFolderPath));
	const aiScene* pScene = importer.ReadFile(pszXmlFilePath, 0);
	if (!pScene) {
		std::cerr << "Failed during import" << std::endl;
		strncpy(pszError, importer.GetErrorString(), iMaxErrorSize);
		return false;
	}
	aiScene* copiedScene;
	aiCopyScene(pScene, &copiedScene);

	Assimp::Exporter exporter;
	aiReturn result = exporter.Export(copiedScene, "collada", pszDaeFilePath);
	if (result != aiReturn_SUCCESS) {
	       std::cerr << "Failed during export" << std::endl;
		strncpy(pszError, exporter.GetErrorString(), iMaxErrorSize);
		return false;
	}

	return true;
}
bool ConvertDaeToXml(const char* pszGameBaseFolderPath,
		const char* pszDaeFilePath, const char* pszXmfFilePath, char* pszError,
		int iMaxErrorSize) {
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(pszDaeFilePath, 0);
	if (!pScene) {
		std::cerr << "Failed during import" << std::endl;
		strncpy(pszError, importer.GetErrorString(), iMaxErrorSize);
		return false;
	}

	Assimp::Exporter exporter;
	exporter.RegisterExporter(XmfExporter::Format);
	XmfExporter::GameBaseFolderPath = pszGameBaseFolderPath;
	aiReturn result = exporter.Export(pScene, "xmf", pszXmfFilePath);
	if (result != aiReturn_SUCCESS) {
        std::cerr << "Failed during export" << std::endl;
		strncpy(pszError, exporter.GetErrorString(), iMaxErrorSize);
		return false;
	}

	return true;
}

extern "C" bool C_API_ConvertXmlToDae(const char* pszGameBaseFolderPath,
		const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError,
		int iMaxErrorSize) {
	return ConvertXmlToDae(pszGameBaseFolderPath, pszXmlFilePath,
			pszDaeFilePath, pszError, iMaxErrorSize);
}

extern "C" bool C_API_ConvertDaeToXml(const char* pszGameBaseFolderPath,
		const char* pszDaeFilePath, const char* pszXmfFilePath, char* pszError,
		int iMaxErrorSize) {
	return ConvertXmlToDae(pszGameBaseFolderPath, pszDaeFilePath,
			pszXmfFilePath, pszError, iMaxErrorSize);
}
//
//extern "C" bool ConvertXacToDae ( const char* pszGameBaseFolderPath, const char* pszXacFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize )
//{
//    Assimp::Importer importer;
//    importer.RegisterLoader ( new XacImporter ( pszGameBaseFolderPath ) );
//    const aiScene* pScene = importer.ReadFile ( pszXacFilePath, 0 );
//    if ( !pScene )
//    {
//       strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
//        return false;
//    }
//
//    Assimp::Exporter exporter;
//    aiReturn result = exporter.Export ( pScene, "collada", pszDaeFilePath );
//    if ( result != aiReturn_SUCCESS )
//    {
//       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize );
//        return false;
//    }
//
//    return true;
//}
//
//extern "C" bool ConvertDaeToXac ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXacFilePath, char* pszError, int iMaxErrorSize )
//{
//    Assimp::Importer importer;
//    const aiScene* pScene = importer.ReadFile ( pszDaeFilePath, 0 );
//    if ( !pScene )
//    {
//       strncpy( pszError, importer.GetErrorString (), iMaxErrorSize );
//        return false;
//    }
//
//    Assimp::Exporter exporter;
//    exporter.RegisterExporter ( XacExporter::Format );
//    XacExporter::GameBaseFolderPath = pszGameBaseFolderPath;
//    aiReturn result = exporter.Export ( pScene, "xac", pszXacFilePath );
//    if ( result != aiReturn_SUCCESS )
//    {
//       strncpy( pszError, exporter.GetErrorString (), iMaxErrorSize );
//        return false;
//    }
//
//    return true;
//}
