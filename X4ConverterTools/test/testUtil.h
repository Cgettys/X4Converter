//
// Created by cg on 4/11/19.
//

#pragma once

#include <pugixml.hpp>
#include <string>
#include <assimp/scene.h>
#include <X4ConverterTools/xmf/XmfFile.h>
#include <boost/filesystem.hpp>
namespace test {
class TestUtil {
 public:
  static void checkAiNodeName(aiNode *node, std::string name, std::string qualifier = "");

  static std::unique_ptr<pugi::xml_document> GetXmlDocument(const std::string &path);

  static boost::filesystem::path GetBasePath();

  static ConversionContext::Ptr GetTestContext(const boost::filesystem::path &tgtPath, bool convert = true);
  static void CompareXMLFiles(pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc);
  static void checkQuaternion(std::string path, aiQuaternion &expectedQuat, aiQuaternion &actualQuat);
  static void checkPath(const std::string &path, pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc);

  static void checkXuMeshFileEquality(xmf::XmfFile &lhs, xmf::XmfFile &rhs);

  static void checkXmfHeaderEquality(xmf::XmfFile &lFile, xmf::XmfFile &rFile);

  static aiNode *makeAiNode(std::string name, std::string qualifier);

  static std::string makeQualifiedName(std::string name, std::string qualifier);

};
}