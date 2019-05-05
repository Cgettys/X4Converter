#pragma once

#include <string>

bool ConvertXmlToDae(const std::string &pszGameBaseFolderPath, const std::string &pszXmlFilePath,
                     const std::string &pszDaeFilePath);

bool ConvertDaeToXml(const std::string &pszGameBaseFolderPath, const std::string &pszDaeFilePath,
                     const std::string &pszXmlFilePath);
