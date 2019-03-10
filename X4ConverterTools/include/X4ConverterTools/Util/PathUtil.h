#pragma once

#include <string>
#include <iostream>
#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <X4ConverterTools/Types.h>

class PathUtil {
public:
    static boost::filesystem::path
    GetRelativePath(const boost::filesystem::path &filePath, const boost::filesystem::path &relativeToFolderPath);
};
