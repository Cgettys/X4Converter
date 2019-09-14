#include <boost/test/unit_test.hpp>

#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;// NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(UnitTests)

BOOST_AUTO_TEST_SUITE(PartUnitTests)

// TODO split all these sorta unit tests in half in all files
BOOST_AUTO_TEST_CASE(from_xml_read_part_name_correct) {
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto partNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
  partNode.remove_child("lods");
  BOOST_TEST_REQUIRE(!partNode.empty());
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  auto part = Part(partNode, ctx);
  BOOST_TEST(part.getName() == "anim_main");
}

BOOST_AUTO_TEST_CASE(xml_to_ainode_read_part_name_correct) {
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto partNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
  partNode.remove_child("lods");
  BOOST_TEST_REQUIRE(!partNode.empty());
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  auto part = Part(partNode, ctx);
  auto result = part.ConvertToAiNode(pugi::xml_node());
  BOOST_TEST(std::string(result->mName.C_Str()) == "anim_main");
  delete result;
}

BOOST_AUTO_TEST_CASE(from_xml_read_part_name_throws_on_empty) {
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto partNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
  BOOST_TEST_REQUIRE(!partNode.empty());
  partNode.remove_attribute("name");
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  BOOST_CHECK_THROW(auto part = Part(partNode, ctx), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(from_xml_name_throws_on_wrong_type) {
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto partNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts[1]").node();
  BOOST_TEST_REQUIRE(!partNode.empty());
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  BOOST_CHECK_THROW(auto part = Part(partNode, ctx), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(from_ai_node_part_name) {
  std::string partName = "testpart";
  auto ainode = new aiNode(partName);
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  Part part(ctx);
  part.ConvertFromAiNode(ainode, pugi::xml_node());
  std::string name = part.getName();
  BOOST_TEST(name == partName);
  delete ainode;
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_name) {
  std::string partName = "testpart";
  auto ainode = new aiNode(partName);
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  Part part(ctx);
  part.ConvertFromAiNode(ainode, pugi::xml_node());
  pugi::xml_document doc;
  auto node = doc.append_child("parts");
  part.ConvertToGameFormat(node);

  std::string actualName = node.child("part").attribute("name").value();
  BOOST_TEST(partName == actualName);

  delete ainode;
}

// TODO rewriteme
//        BOOST_AUTO_TEST_CASE(xml_to_ainode_read_ref) {
//            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
//            auto partNode = doc->select_node(
//                    "/components/component/connections/connection[@name='Connection35']/parts/part").node();
//            BOOST_TEST_REQUIRE(!partNode.empty());
//            auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");
//
//            auto part = Part(partNode, ctx);
//
//            auto result = part.ConvertToAiNode();
//            TestUtil::checkAiNodeName(result, "anim_thruster_06");
//            BOOST_TEST_REQUIRE(result->mNumChildren == 1);
//            TestUtil::checkAiNodeName(result->mChildren[0],
//                                      "anim_thruster_06|DO_NOT_EDIT.ref|thruster_ship_s_01.anim_thruster_001");
//            delete doc;
//            delete result;
//        }
//        BOOST_AUTO_TEST_CASE(ainode_to_xml_write_ref) {
//            std::string partName = "anim_thruster_06";
//            std::string childName = "anim_thruster_06|DO_NOT_EDIT.ref|thruster_ship_s_01.anim_thruster_001";
//            auto node = new aiNode(partName);
//            auto children = new aiNode *[1];
//            children[0] = new aiNode(childName);
//            node->addChildren(1, children);
//            auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");
//
//            Part part(ctx);
//            part.ConvertFromAiNode(node);
//            pugi::xml_document doc;
//            auto outNode = doc.append_child("parts");
//            part.ConvertToGameFormat(outNode);
//
//            auto partNode = outNode.child("part");
//            std::string ref = partNode.attribute("ref").value();
//            BOOST_TEST("thruster_ship_s_01.anim_thruster_001" == ref);
//            delete node;
//            delete[] children;
//        }



//        BOOST_AUTO_TEST_CASE(ainode_to_xml_to_read_ref) { 
//            auto node = new aiNode("Connection35");
//        }
// TODO global naming constraints, wrecks, uv_animation
// TODO does a collision mesh always exist?
// TODO size, sizeraw,pivot, sounds, effectemmiters etc

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(IntegrationTests)
BOOST_AUTO_TEST_SUITE(PartIntegrationTests)

BOOST_AUTO_TEST_CASE(xml_to_ainode_lods) {
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto partNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
  BOOST_TEST_REQUIRE(!partNode.empty());
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  auto part = Part(partNode, ctx);

  auto result = part.ConvertToAiNode(pugi::xml_node());
  BOOST_TEST(std::string(result->mName.C_Str()) == "anim_main");
  BOOST_TEST_REQUIRE(result->mNumChildren == 6);
  TestUtil::checkAiNodeName(result->mChildren[0], "anim_main|wreck|anim_main_wreck");
  TestUtil::checkAiNodeName(result->mChildren[1], "anim_main|collision");
  TestUtil::checkAiNodeName(result->mChildren[2], "anim_main|lod0");
  TestUtil::checkAiNodeName(result->mChildren[3], "anim_main|lod1");
  TestUtil::checkAiNodeName(result->mChildren[4], "anim_main|lod2");
  TestUtil::checkAiNodeName(result->mChildren[5], "anim_main|lod3");
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_lods) {
  std::string partName = "testpart";
  auto ainode = new aiNode(partName);
  auto ainodeChildren = new aiNode *[3];
  ainodeChildren[0] = new aiNode(partName + "|collision");
  ainodeChildren[1] = new aiNode(partName + "|lod1");
  ainodeChildren[2] = new aiNode(partName + "|lod2");
  ainode->addChildren(3, ainodeChildren);
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01_data)");

  Part part(ctx);
  part.ConvertFromAiNode(ainode, pugi::xml_node());
  pugi::xml_document doc;
  auto node = doc.append_child("parts");
  part.ConvertToGameFormat(node);

  BOOST_TEST(!node.child("part").child("lods").empty());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END() 