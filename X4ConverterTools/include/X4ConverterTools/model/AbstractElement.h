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

  virtual void ConvertToGameFormat(pugi::xml_node &out) = 0;

 protected:
  static void CheckXmlNode(pugi::xml_node &src, const std::string expectedName);
  void ReadOffset(pugi::xml_node target);

  void WriteOffset(pugi::xml_node target);

  ConversionContext::MetadataMap attrs;

  ConversionContext::Ptr ctx;
  aiVector3D offsetPos;
  aiQuaternion offsetRot;
 private:
  std::string name;

};

class AiNodeElement : public AbstractElement {
 public:
  explicit AiNodeElement(ConversionContext::Ptr ctx);
  virtual ~AiNodeElement() = default;
  virtual aiNode *ConvertToAiNode() = 0;
  virtual void ConvertFromAiNode(aiNode *node) = 0;
 protected:
  void ApplyOffsetToAiNode(aiNode *target);

};
}