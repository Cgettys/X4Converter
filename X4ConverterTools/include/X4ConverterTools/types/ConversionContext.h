#pragma once
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <X4ConverterTools/types.h>
struct ConversionContext {
    std::map<std::string, uint32_t> Materials;
    std::vector<aiMesh *> Meshes;
};