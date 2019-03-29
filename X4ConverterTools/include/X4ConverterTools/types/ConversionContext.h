#pragma once
#include <map>
#include <vector>
#include <assimp/scene.h>
struct ConversionContext {
    std::map<std::string, unsigned int> Materials;
    std::vector<aiMesh *> Meshes;
};