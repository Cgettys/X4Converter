#pragma once

#include <string>
#include <X4ConverterTools/Material.h>
#include <X4ConverterTools/MaterialCollection.h>
#include <X4ConverterTools/types.h>

class MaterialLibrary {
    friend Material;

public:
    explicit MaterialLibrary(const std::string &gameBaseFolderPath);

    const MaterialCollection &GetCollection(const std::string &name);

    Material *GetMaterial(const std::string &dottedName);

private:
    std::string _gameBaseFolderPath;
    std::map<std::string, MaterialCollection> collections;
};
