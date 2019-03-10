#pragma once
// TODO remove when possible
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>

#include <X4ConverterTools/Xmf/XmfImporter.h>
#include <X4ConverterTools/Xmf/XmfExporter.h>
bool ConvertXmlToDae ( const char* pszGameBaseFolderPath, const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
bool ConvertDaeToXml ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXmlFilePath, char* pszError, int iMaxErrorSize );

extern "C" bool C_API_ConvertXmlToDae ( const char* pszGameBaseFolderPath, const char* pszXmlFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
extern "C" bool C_API_ConvertDaeToXml ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXmlFilePath, char* pszError, int iMaxErrorSize );

//extern "C" bool ConvertXacToDae ( const char* pszGameBaseFolderPath, const char* pszXacFilePath, const char* pszDaeFilePath, char* pszError, int iMaxErrorSize );
//extern "C" bool ConvertDaeToXac ( const char* pszGameBaseFolderPath, const char* pszDaeFilePath, const char* pszXacFilePath, char* pszError, int iMaxErrorSize );
