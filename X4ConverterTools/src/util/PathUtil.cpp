#include <X4ConverterTools/util/PathUtil.h>

using namespace boost;
using namespace boost::filesystem;

std::string PathUtil::MakePlatformSafe(const std::string &filePath) {
    if (filePath.empty()) {
        std::cerr << "Warning, MakePlatformSafe received an empty file path as an argument." << std::endl;
        return filePath;
    }
    std::string result(filePath);

    std::string preferredSep;
    preferredSep.append(1, path::preferred_separator);
    //TODO check for C://?
    algorithm::replace_all(result, "\\", preferredSep);
    algorithm::replace_all(result, "/", preferredSep);
    if (result.empty()) {
        throw std::runtime_error("Empty path after path made platform safe");
    }
    return result;
}

std::string PathUtil::MakeGameSafe(const std::string &filePath) {
    std::string result(filePath);
    algorithm::replace_all(result, "//", "\\");
    return result;
}

path PathUtil::MakePlatformSafe(const path &filePath) {
    return path(MakePlatformSafe(filePath.string()));
}

path PathUtil::MakeGameSafe(const path &filePath) {
    return path(MakeGameSafe(filePath.string()));
}

path PathUtil::GetRelativePath(const path &filePath, const path &relativeToFolderPath) {
    if (filePath.is_relative()) {
        throw std::runtime_error("filePath is already relative");
    }
    if (!relativeToFolderPath.is_absolute()) {
        throw std::runtime_error("relativeToPath is not absolute");
    }
    std::vector<std::string> filePathParts;

    split(filePathParts, filePath.string(), [](char c) { return c == '/' || c == '\\'; });


    std::vector<std::string> relativeToFolderPathParts;
    split(relativeToFolderPathParts, relativeToFolderPath.string(), [](char c) { return c == '/' || c == '\\'; });

    //https://stackoverflow.com/a/17270869
    filePathParts.erase(std::remove(filePathParts.begin(), filePathParts.end(), ""), filePathParts.end());

    relativeToFolderPathParts.erase(std::remove(relativeToFolderPathParts.begin(), relativeToFolderPathParts.end(), ""),
                                    relativeToFolderPathParts.end());
// TODO debug flag to control this
    //    for (auto s : filePathParts){
//        std::cout << s<<",";
//    }
//    std::cout <<std::endl;
//
//    for (auto s : relativeToFolderPathParts){
//        std::cout << s<< ",";
//    }
//    std::cout <<std::endl;
    int differenceStart = 0;
    while (differenceStart < filePathParts.size() - 1 && differenceStart < relativeToFolderPathParts.size() &&
           iequals(filePathParts[differenceStart], relativeToFolderPathParts[differenceStart])) {
        differenceStart++;
    }

    if (differenceStart == 0) {
        std::cerr << "Paths lack common root" << std::endl;
        throw std::runtime_error(str(format("Paths do not have a common root %1% %2%") % filePath.c_str() %
                                     relativeToFolderPath.c_str()));
    }
    path result;
    for (int i = differenceStart; i < relativeToFolderPathParts.size(); ++i) {
        result /= "..";
    }

    for (int i = differenceStart; i < filePathParts.size() - 1; ++i) {
        result /= filePathParts[i];
    }

    result /= filePath.filename();
//    std::cout << "Returned path:"<< filePath << " " << relativeToFolderPath << " "<<  result.string() << std::endl;
    return result;
}


std::string PathUtil::GetOutputPath(const std::string &inputPath) {

#ifdef X4ConverterTools_TEST
    std::cout << "Using test paths" << std::endl;
    path p(inputPath);
    std::string ext = ".out";
    ext.append(p.extension().string());
    p.replace_extension(ext);
    return p.generic_string();
#else
    return inputPath;
#endif
}

