#pragma once

#include <string>
#include <map>
#include <functional>
#include <assimp/scene.h>
#include <X4ConverterTools/ConversionContext.h>
#include <X4ConverterTools/util/XmlUtil.h>

namespace model {
class AbstractElement {
 public:
  explicit AbstractElement(ConversionContext::Ptr ctx, std::string qualifier);

  virtual ~AbstractElement() = default;

  std::string getQualifiedName();

  static bool matchesQualifier(const std::string &n, const std::string &q);

  std::string parseQualifiedName(const std::string &n);

  std::string getName();

  void setName(std::string n);

  virtual aiNode *ConvertToAiNode() {
    auto *result = new aiNode;
    result->mName = aiString{getName()};
    return result;
  }
  virtual void ConvertFromAiNode(aiNode *node) {
    setName(node->mName.C_Str());
  }

  virtual void ConvertToGameFormat(pugi::xml_node &out) = 0;
  bool hasAttr(const std::string &n);
  std::string getAttr(const std::string &n);
  void setAttr(const std::string &n, const std::string &value, bool overwrite = true);
 protected:
  static auto ExcludePredicate(const std::string &excluded) {
    return [excluded](auto n) -> bool {
      return n != excluded;
    };
  }
  void WriteAttrs(pugi::xml_node &node);
  void WriteAttrs(pugi::xml_node &node, const std::function<bool(std::string)> &predicate);
  void ProcessAttributes(pugi::xml_node &node,
                         const std::function<void(AbstractElement *,
                                                  std::string,
                                                  std::string)> &func = &AbstractElement::DefaultProcessAttribute);
  void DefaultProcessAttribute(const std::string &n, const std::string &value);
  ConversionContext::Ptr ctx;
  static std::vector<aiNode *> getChildren(aiNode *node);

  void CheckXmlElement(pugi::xml_node &src, const std::string &expectedName, bool nameRequired = true);

 private:
  std::string name;
  std::string qualifier;

};

}