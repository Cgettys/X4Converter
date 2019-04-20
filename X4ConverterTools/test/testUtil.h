//
// Created by cg on 4/11/19.
//

#pragma once

#include <pugixml.hpp>
#include <string>
#include <assimp/scene.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>
namespace test {

    class TestUtil {
    public:
        static void checkAiNodeName(aiNode *node, std::string name);

        static pugi::xml_document* GetXmlDocument(std::string path);
        static std::string GetBasePath();

        static void CompareXMLFiles(pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc);

        static void checkXuMeshFileEquality(xmf::XuMeshFile &lhs, xmf::XuMeshFile &rhs);

        static void checkXmfHeaderEquality(xmf::XuMeshFile &lFile, xmf::XuMeshFile &rFile);

    };
}