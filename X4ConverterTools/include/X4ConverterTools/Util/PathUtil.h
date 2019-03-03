#pragma once

class PathUtil
{
public:
    static boost::filesystem::path      GetRelativePath     ( const boost::filesystem::path& filePath, const boost::filesystem::path& relativeToFolderPath );
};
