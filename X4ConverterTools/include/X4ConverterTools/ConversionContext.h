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

    Assimp::IOStream *GetSourceFile(const std::string &name, std::string mode = "rb");

    std::map<std::string, uint32_t> Materials;
    std::string gameBaseFolderPath;

    void SetScene(aiScene *pScene);

    aiMesh *GetMesh(int meshIndex);

    aiLight *GetLight(const std::string &name);

    void AddMesh(aiNode *parentNode, aiMesh *mesh);
    void AddLight(aiLight *light);

    bool CheckLight(const std::string &name);

protected:
    std::shared_ptr<Assimp::IOSystem> io;
    aiScene *pScene;
    std::vector<aiMesh *> meshes;
    std::string sourcePathSuffix;
    model::MaterialLibrary materialLibrary;
    std::map<std::string, aiLight *> lights;

};