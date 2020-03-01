#pragma once
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <assimp/scene.h>
#include <assimp/DefaultIOSystem.h>
#include <boost/filesystem.hpp>
#include <pugixml.hpp>
namespace util {
class FileSystemUtil {
 public:
  using Ptr=std::shared_ptr<FileSystemUtil>;
  explicit FileSystemUtil(std::string gameBaseFolderPath,
                          bool test,
                          std::shared_ptr<Assimp::IOSystem> io = std::make_shared<Assimp::DefaultIOSystem>());
  [[nodiscard]] static std::string MakePlatformSafe(const std::string &filePath);
  [[nodiscard]] static std::string MakeGameSafe(const std::string &filePath);
  [[nodiscard]] static boost::filesystem::path MakePlatformSafe(const boost::filesystem::path &filePath);
  [[nodiscard]] static boost::filesystem::path MakeGameSafe(const boost::filesystem::path &filePath);

  [[nodiscard]] boost::filesystem::path GetRelativePath(const boost::filesystem::path &filePath);
  [[nodiscard]] boost::filesystem::path GetRelativePath(const boost::filesystem::path &filePath,
                                                        const boost::filesystem::path &relativeToFolderPath);
  [[nodiscard]] boost::filesystem::path GetAbsolutePath(const boost::filesystem::path &filePath);

  void LoadXml(pugi::xml_document &doc, std::string path);
  std::string GetDaeOutputPath(std::string path);
  [[nodiscard]] std::string GetOutputPath(const std::string &inputPath);
  void SetSourcePathSuffix(std::string path);

  std::string GetSourcePath();

  Assimp::IOStream *GetSourceFile(const std::string &name, const std::string &mode = "rb");
 private:
  bool relative_paths = false;
  bool test;
  bool hasPrintedUsingTestPathsWarning = false;
  std::string sourcePathSuffix;
  boost::filesystem::path gameBaseFolderPath;
  std::shared_ptr<Assimp::IOSystem> io;
};
}

