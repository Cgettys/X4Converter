#pragma once
#include <assimp/scene.h>
#include <pugixml.hpp>
namespace model {
class Offset {
 public:
  Offset() = default;
  explicit Offset(pugi::xml_node &source);
  void ReadXml(pugi::xml_node &source);
  void WriteXml(pugi::xml_node &target);
  void ReadAiNode(aiNode *source);
  void WriteAiNode(aiNode *target);
  void ReadAiLight(aiLight *light);
  void WriteAiLight(aiLight *light);
 private:
  aiVector3D offsetPos{0, 0, 0};
  aiQuaternion offsetRot{0, 0, 0, 0};
};
}