#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>


#include "X4ConverterTools/model/MaterialLibrary.h"

class ConversionContext {
public:
    explicit ConversionContext(const std::string &gameBaseFolderPath);

    void AddMaterialsToScene(const std::string &filePath, aiScene *pScene);

    std::map<std::string, uint32_t> Materials;
    std::vector<aiMesh *> Meshes;
    std::string gameBaseFolderPath;
private:

    model::MaterialLibrary materialLibrary;
};