#pragma once

#include <string>
#include <X4ConverterTools/ConversionContext.h>

bool ConvertXmlToDae(const ConversionContext::Ptr &ctx, const std::string &xmlFilePath, const std::string &daeFilePath);

bool ConvertDaeToXml(const ConversionContext::Ptr &ctx, const std::string &daeFilePath, const std::string &xmlFilePath);
