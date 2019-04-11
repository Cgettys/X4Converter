#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include "MaterialLibrary.h"

class ConversionContext {
public:
    ConversionContext(std::string gameBaseFolderPath);

    void AddMaterialsToScene(const std::string &filePath, aiScene *pScene);

    std::map<std::string, uint32_t> Materials;
    std::vector<aiMesh *> Meshes;
    std::string gameBaseFolderPath;
private:

    MaterialLibrary materialLibrary;
};