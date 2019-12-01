//
// Created by cg on 4/11/19.
//

#pragma once

#include <pugixml.hpp>
#include <string>
#include <assimp/scene.h>
#include <X4ConverterTools/xmf/XmfFile.h>
namespace test {

class TestUtil {
 public:
  static void checkAiNodeName(aiNode *node, std::string name);

  static std::unique_ptr<pugi::xml_document> GetXmlDocument(std::string path);

  static std::string GetBasePath();

  static ConversionContext::Ptr GetTestContext(std::string geomPath, bool convert = true);
  static void CompareXMLFiles(pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc);

  static void checkXuMeshFileEquality(xmf::XmfFile &lhs, xmf::XmfFile &rhs);

  static void checkXmfHeaderEquality(xmf::XmfFile &lFile, xmf::XmfFile &rFile);

};
}