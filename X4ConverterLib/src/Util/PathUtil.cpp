#include "StdInc.h"

using namespace boost;
using namespace boost::filesystem;

path PathUtil::GetRelativePath ( const path& filePath, const path& relativeToFolderPath )
{
    if ( filePath.is_relative () )
        throw std::string ( "filePath is already relative" );

    if ( !relativeToFolderPath.is_absolute () )
        throw std::string ( "relativeToPath is not absolute" );

    std::vector < std::string > filePathParts;
    split ( filePathParts, filePath.string (), [](char c) { return c == '\\'; });

    std::vector < std::string > relativeToFolderPathParts;
    split ( relativeToFolderPathParts, relativeToFolderPath.string (), [](char c) { return c == '\\'; });

    int differenceStart = 0;
    while ( differenceStart < filePathParts.size() - 1 && differenceStart < relativeToFolderPathParts.size() &&
            iequals(filePathParts[differenceStart], relativeToFolderPathParts[differenceStart]) )
    {
        differenceStart++;
    }

    if ( differenceStart == 0 )
        throw std::string ( "Paths do not have a common root" );

    path result;
    for ( int i = differenceStart; i < relativeToFolderPathParts.size (); ++i )
    {
        result /= "..";
    }

    for ( int i = differenceStart; i < filePathParts.size () - 1; ++i )
    {
        result /= filePathParts[i];
    }

    result /= filePath.filename ();
    return result;
}
