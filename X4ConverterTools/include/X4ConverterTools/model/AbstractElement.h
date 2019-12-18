//
// Created by cg on 4/14/19.
//

#pragma once

#include <string>
#include <map>
#include <assimp/scene.h>
#include <X4ConverterTools/ConversionContext.h>
#include <X4ConverterTools/util/XmlUtil.h>

namespace model {
class AbstractElement {
 public:
  explicit AbstractElement(ConversionContext::Ptr ctx);

  virtual ~AbstractElement() = default;

  std::string getName();

  void setName(std::string n);

  virtual aiNode *ConvertToAiNode() = 0;

  virtual void ConvertFromAiNode(aiNode *node) = 0;

  virtual void ConvertToGameFormat(pugi::xml_node &out) = 0;

 protected:

  // TODO template pattern?

  virtual void populateAiNodeChildren(aiNode *target, std::vector<aiNode *> children);

  void ReadOffset(pugi::xml_node target);

  void ApplyOffsetToAiNode(aiNode *target);

  void WriteOffset(pugi::xml_node target);


  ConversionContext::MetadataMap attrs;

  ConversionContext::Ptr ctx;
 protected:

  aiVector3D offsetPos;
  aiQuaternion offsetRot;
 private:
  std::string name;

};

}