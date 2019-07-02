#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>


#include <assimp/DefaultIOSystem.h>
#include "X4ConverterTools/model/MaterialLibrary.h"

class ConversionContext {
public:
    explicit ConversionContext(const std::string &gameBaseFolderPath, std::shared_ptr<Assimp::IOSystem> io);

    static boost::filesystem::path
    GetRelativePath(const boost::filesystem::path &filePath, const boost::filesystem::path &relativeToFolderPath);


    static std::string MakePlatformSafe(const std::string &filePath);

    static std::string MakeGameSafe(const std::string &filePath);

    static boost::filesystem::path MakePlatformSafe(const boost::filesystem::path &filePath);

    static boost::filesystem::path MakeGameSafe(const boost::filesystem::path &filePath);


    static std::string GetOutputPath(const std::string &inputPath);
    void PopulateSceneArrays();

    void SetSourcePathSuffix(std::string path);

    std::string GetSourcePath();

    Assimp::IOStream *GetSourceFile(std::string name, std::string mode = "rb");

    std::map<std::string, uint32_t> Materials;
    std::vector<aiMesh *> Meshes;
    std::string gameBaseFolderPath;
    aiScene *pScene;

    std::vector<aiLight *> GetLights();

    aiLight *GetLight(std::string name);

    void AddLight(aiLight *light);
protected:
    std::shared_ptr<Assimp::IOSystem> io;
private:
    std::string sourcePathSuffix;
    model::MaterialLibrary materialLibrary;
    std::map<std::string, aiLight *> lights;
};