#pragma once

#include <cstring>
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
bool ConvertXmlToDae(const std::string &pszGameBaseFolderPath, const std::string &pszXmlFilePath,
                     const std::string &pszDaeFilePath);

bool ConvertDaeToXml(const std::string &pszGameBaseFolderPath, const std::string &pszDaeFilePath,
                     const std::string &pszXmlFilePath);
