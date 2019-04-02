#pragma once

#include <string>
#include <iostream>
#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <X4ConverterTools/types.h>

class PathUtil {
public:
    static boost::filesystem::path
    GetRelativePath(const boost::filesystem::path &filePath, const boost::filesystem::path &relativeToFolderPath);


    static std::string MakePlatformSafe(const std::string &filePath);

    static std::string MakeGameSafe(const std::string &filePath);

    static boost::filesystem::path MakePlatformSafe(const boost::filesystem::path &filePath);

    static boost::filesystem::path MakeGameSafe(const boost::filesystem::path &filePath);


    static std::string GetOutputPath(const std::string &inputPath);
};
