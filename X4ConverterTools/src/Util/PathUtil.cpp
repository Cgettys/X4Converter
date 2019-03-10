#include <X4ConverterTools/Util/PathUtil.h>

using namespace boost;
using namespace boost::filesystem;

path PathUtil::GetRelativePath(const path &filePath,
                               const path &relativeToFolderPath) {
    if (filePath.is_relative())
        throw std::runtime_error("filePath is already relative");

    if (!relativeToFolderPath.is_absolute())
        throw std::runtime_error("relativeToPath is not absolute");

    std::vector<std::string> filePathParts;

#ifdef BOOST_OS_LINUX
    const char sep = '/';
#else
    const char sep = '\\';
#endif
    split(filePathParts, filePath.string(), [](char c) { return c == sep; });

    std::vector<std::string> relativeToFolderPathParts;
    split(relativeToFolderPathParts, relativeToFolderPath.string(),
          [](char c) { return c == sep; });

    int differenceStart = 0;
    while (differenceStart < filePathParts.size() - 1
           && differenceStart < relativeToFolderPathParts.size()
           && iequals(filePathParts[differenceStart],
                      relativeToFolderPathParts[differenceStart])) {
        differenceStart++;
    }

    if (differenceStart == 0) {
        throw std::runtime_error(
                str(format("Paths do not have a common root %1 %2") % filePath.c_str() % relativeToFolderPath.c_str()));
    }
    path result;
    for (int i = differenceStart; i < relativeToFolderPathParts.size(); ++i) {
        result /= "..";
    }

    for (int i = differenceStart; i < filePathParts.size() - 1; ++i) {
        result /= filePathParts[i];
    }

    result /= filePath.filename();
    return result;
}
