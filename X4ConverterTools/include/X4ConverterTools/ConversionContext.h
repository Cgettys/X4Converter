#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>


#include <assimp/DefaultIOSystem.h>
#include "X4ConverterTools/model/MaterialLibrary.h"

class ConversionContext {
public:
    explicit ConversionContext(const std::string &gameBaseFolderPath, std::shared_ptr<Assimp::IOSystem> io);

    void PopulateSceneArrays();

    void SetSourcePathSuffix(std::string path);

    std::string GetSourcePath();

    Assimp::IOStream *GetSourceFile(std::string name, std::string mode = "rb");

    std::map<std::string, uint32_t> Materials;
    std::vector<aiMesh *> Meshes;
    std::string gameBaseFolderPath;
    aiScene *pScene;
protected:
    std::shared_ptr<Assimp::IOSystem> io;
private:
    std::string sourcePathSuffix;
    model::MaterialLibrary materialLibrary;
};