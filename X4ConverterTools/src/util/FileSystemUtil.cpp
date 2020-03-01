#include <X4ConverterTools/util/FileSystemUtil.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
namespace util {
namespace fs = boost::filesystem;
namespace algo = boost::algorithm;
using boost::filesystem::path;
FileSystemUtil::FileSystemUtil(std::string gameBaseFolderPath,
                               bool test,
                               std::shared_ptr<Assimp::IOSystem> io) :
    gameBaseFolderPath(gameBaseFolderPath),
    io(std::move(io)),
    test(test) {

}
std::string FileSystemUtil::MakePlatformSafe(const std::string &filePath) {
  if (filePath.empty()) {
    std::cerr << "Warning, MakePlatformSafe received an empty file path as an argument." << std::endl;
    return filePath;
  }
  std::string result(filePath);

  std::string preferredSep;
  preferredSep.append(1, path::preferred_separator);
  //TODO check for C://?
  algo::replace_all(result, "\\", preferredSep);
  algo::replace_all(result, "/", preferredSep);
  if (result.empty()) {
    throw std::runtime_error("Empty path after path made platform safe");
  }
  return result;
}
std::string FileSystemUtil::MakeGameSafe(const std::string &filePath) {
  std::string result(filePath);
  algo::replace_all(result, "//", "\\");
  return result;
}
path FileSystemUtil::MakePlatformSafe(const path &filePath) {
  return path(MakePlatformSafe(filePath.string()));
}
path FileSystemUtil::MakeGameSafe(const path &filePath) {
  return path(MakeGameSafe(filePath.string()));
}

path FileSystemUtil::GetAbsolutePath(const path &filePath) {

  if (!gameBaseFolderPath.empty() && filePath.is_relative()) {
    std::cout << "Prepending " << gameBaseFolderPath << " to path " << filePath << std::endl;
    return gameBaseFolderPath / filePath;
  }
  return filePath.generic_path();
}

path FileSystemUtil::GetRelativePath(const path &filePath) {
  return GetRelativePath(filePath, gameBaseFolderPath);
}

path FileSystemUtil::GetRelativePath(const path &filePath, const path &relativeToFolderPath) {
  if (filePath.is_relative()) {
    throw std::runtime_error("filePath is already relative");
  }
  if (!relativeToFolderPath.is_absolute()) {
    throw std::runtime_error("relativeToPath is not absolute");
  }
  std::vector<std::string> filePathParts;

  algo::split(filePathParts, filePath.string(), [](char c) { return c == '/' || c == '\\'; });

  std::vector<std::string> relativeToFolderPathParts;
  algo::split(relativeToFolderPathParts, relativeToFolderPath.string(), [](char c) { return c == '/' || c == '\\'; });

  //https://stackoverflow.com/a/17270869
  filePathParts.erase(std::remove(filePathParts.begin(), filePathParts.end(), ""), filePathParts.end());

  relativeToFolderPathParts.erase(std::remove(relativeToFolderPathParts.begin(), relativeToFolderPathParts.end(), ""),
                                  relativeToFolderPathParts.end());

  int differenceStart = 0;
  while (differenceStart < filePathParts.size() - 1 && differenceStart < relativeToFolderPathParts.size() &&
      algo::iequals(filePathParts[differenceStart], relativeToFolderPathParts[differenceStart])) {
    differenceStart++;
  }

  if (differenceStart == 0) {
    std::cerr << "Paths lack common root" << std::endl;
    throw std::runtime_error(str(boost::format("Paths do not have a common root %1% %2%") % filePath.c_str() %
        relativeToFolderPath.c_str()));
  }
  path result;
  for (auto i = differenceStart; i < relativeToFolderPathParts.size(); ++i) {
    result /= "..";
  }

  for (auto i = differenceStart; i < filePathParts.size() - 1; ++i) {
    result /= filePathParts[i];
  }

  result /= filePath.filename();
//    std::cout << "Returned path:"<< filePath << " " << relativeToFolderPath << " "<<  result.string() << std::endl;
  return result;
}
void FileSystemUtil::LoadXml(pugi::xml_document &doc, std::string path) {
  pugi::xml_parse_result load_result;
  // TODO better way to do extension and path handling / generate a Config object to ease integration testing.
  if (fs::exists(path)) {
    load_result = doc.load_file((path).c_str());
  } else if (fs::exists(gameBaseFolderPath / path)) {
    std::cout << "Paths appear to be relative" << std::endl;
    load_result = doc.load_file((gameBaseFolderPath / path).c_str());
    relative_paths = true;
  }
  if (load_result.status != pugi::status_ok) {
    throw std::runtime_error("xml file could not be opened!");
  }
}
// TODO fold into GetOutputPath somehow?
std::string FileSystemUtil::GetDaeOutputPath(std::string path) {
  return relative_paths ? (gameBaseFolderPath / path).string() : path;
}

std::string FileSystemUtil::GetOutputPath(const std::string &inputPath) {
  if (test) {
    if (!hasPrintedUsingTestPathsWarning) {
      std::cerr << "Note: using test paths" << std::endl;
      hasPrintedUsingTestPathsWarning = true;
    }
    path p(inputPath);
    std::string ext = ".out";
    ext.append(p.extension().string());
    p.replace_extension(ext);
    return p.generic_string();
  } else {
    return inputPath;
  }

}

void FileSystemUtil::SetSourcePathSuffix(std::string path) {
  if (path == "_data") {
    throw std::runtime_error("Source Path Suffix Malformed");
  }
  std::cout << "Source path suffix set: " << path << std::endl;
  sourcePathSuffix = std::move(path);
}

std::string FileSystemUtil::GetSourcePath() {
  if (sourcePathSuffix.empty()) {
    throw std::runtime_error("Source path suffix has not been set!");
  }
  return FileSystemUtil::MakePlatformSafe(gameBaseFolderPath / sourcePathSuffix).string();
}

Assimp::IOStream *FileSystemUtil::GetSourceFile(const std::string &name, const std::string &mode) {

  std::string path = FileSystemUtil::MakePlatformSafe(GetSourcePath() + "/" + algo::to_lower_copy(name));
  if (!io->Exists(path) && mode != "wb") {
    throw std::runtime_error("File missing: " + name + " Looked at: " + path);
  }

  Assimp::IOStream *result = io->Open(path, mode);
  if (result == nullptr) {
    std::string human_readable_op;
    if (mode == "wb") {
      throw std::runtime_error("File could not be opened for writing: " + name + " Path used: " + path);
    } else if (mode == "rb") {
      throw std::runtime_error("File could not be opened for reading: " + name + " Path used: " + path);
    } else {
      throw std::runtime_error("File could not be opened for " + mode + ": " + name + " Path used: " + path);
    }
  }
  return result;
}

}