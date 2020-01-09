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
  explicit MaterialLibrary(std::string gameBaseFolderPath);

  Material &GetMaterial(const std::string &dottedName);

 private:
  void init();
  // Note lack of thread safety
  bool initialized = false;
  std::regex materialPattern{R"((\w+)\.(\w+))"};
  std::string _gameBaseFolderPath;
  std::unordered_map<std::string, MaterialCollection> collections;
};
}