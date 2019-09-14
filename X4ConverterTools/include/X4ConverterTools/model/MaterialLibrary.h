#pragma once

#include <string>
#include <regex>
#include <X4ConverterTools/model/Material.h>
#include <X4ConverterTools/model/MaterialCollection.h>
#include <X4ConverterTools/types.h>

namespace model {
class MaterialLibrary {
  friend Material;

 public:
  explicit MaterialLibrary(const std::string &gameBaseFolderPath);

  Material *GetMaterial(const std::string &dottedName);

 private:
  std::regex materialPattern{R"((\w+)\.(\w+))"};
  std::string _gameBaseFolderPath;
  std::map<std::string, MaterialCollection> collections;
};
}