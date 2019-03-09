#include <X4ConverterTools/StdInc.h>
#include <assimp/postprocess.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

bool ConvertXmlToDae(const char* pszGameBaseFolderPath,
		const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError,
		int iMaxErrorSize) {

	XmfImporter *ourImporter = new XmfImporter(pszGameBaseFolderPath);
	// TODO fix the leak introduced by fixing the double free
	Assimp::Importer importer;
	importer.RegisterLoader(ourImporter);
	// aiProcess_ValidateDataStructure |
	const aiScene* pScene = importer.ReadFile(pszXmlFilePath, aiProcess_GenNormals | aiProcess_FindDegenerates |
			aiProcess_CalcTangentSpace );
	if (!pScene) {
		std::cerr << "Failed during import" << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;
		strncpy(pszError, importer.GetErrorString(), iMaxErrorSize);
		return false;
	}
	Assimp::Exporter exporter;
	aiReturn result = exporter.Export(pScene, "collada", pszDaeFilePath);
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
