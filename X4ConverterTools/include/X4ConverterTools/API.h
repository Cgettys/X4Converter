#pragma once
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/SceneCombiner.h>

#include <X4ConverterTools/xmf/XmfImporter.h>
#include <X4ConverterTools/xmf/XmfExporter.h>

bool ConvertXmlToDae(const char *pszGameBaseFolderPath, const char *pszXmlFilePath, const char *pszDaeFilePath,
                     char *pszError, int iMaxErrorSize);

bool ConvertDaeToXml(const char *pszGameBaseFolderPath, const char *pszDaeFilePath, const char *pszXmlFilePath,
                     char *pszError, int iMaxErrorSize);
