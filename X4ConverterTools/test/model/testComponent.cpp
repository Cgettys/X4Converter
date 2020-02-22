#include <boost/test/unit_test.hpp>

#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/model/Component.h>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(ComponentUnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_CASE(from_xml_basic) { // NOLINT(cert-err58-cpp)
  auto tgt = "assets/units/size_s/ship_arg_s_fighter_01.xml";
  auto ctx = TestUtil::GetTestContext(tgt);
  auto doc = TestUtil::GetXmlDocument(tgt);
  auto node = doc->root();;
  auto componentNode = node.child("components").child("component");
  BOOST_TEST_REQUIRE(!componentNode.empty());

  auto component = Component(componentNode, ctx);
  BOOST_TEST(component.getName() == "ship_arg_s_fighter_01");
  BOOST_TEST(component.getNumberOfConnections() == 74);
}

BOOST_AUTO_TEST_CASE(xml_to_ainode_basic) { // NOLINT(cert-err58-cpp)
  auto tgt = "assets/units/size_s/ship_arg_s_fighter_01.xml";
  auto ctx = TestUtil::GetTestContext(tgt);
  auto doc = TestUtil::GetXmlDocument(tgt);
  auto node = doc->root();
  auto componentNode = node.child("components").child("component");
  componentNode.remove_child("connections");
  BOOST_TEST_REQUIRE(!componentNode.empty());
  auto component = Component(componentNode, ctx);

  auto result = component.ConvertToAiNode();

  BOOST_TEST_REQUIRE(result->mNumChildren > 0);
  TestUtil::checkAiNodeName(result->mChildren[0], "ship_arg_s_fighter_01");

  delete result;
}

BOOST_AUTO_TEST_CASE(from_ainode_basic) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto node = new aiNode("ship_arg_s_fighter_01");

  auto component = Component(ctx);
  component.ConvertFromAiNode(node);
  BOOST_TEST(component.getName() == "ship_arg_s_fighter_01");
  delete node;
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_complicated) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto node = new aiNode("ship_arg_s_fighter_01");
  auto childrenZero = new aiNode *[2];
  childrenZero[0] = new aiNode("*test_conn_0*");
  childrenZero[1] = new aiNode("*test_conn_1*");
  auto childrenOne = new aiNode *[1];
  childrenOne[0] = new aiNode("test_part_0");
  auto childrenTwo = new aiNode *[1];
  childrenTwo[0] = new aiNode("*test_conn_2*");
  childrenOne[0]->addChildren(1, childrenTwo);
  childrenZero[0]->addChildren(1, childrenOne);
  node->addChildren(2, childrenZero);
  pugi::xml_document doc;
  auto outNode = doc.append_child("components");

  auto component = Component(ctx);
  component.ConvertFromAiNode(node);
  BOOST_TEST(component.getNumberOfConnections() == 3);

  component.ConvertToGameFormat(outNode);

  auto connsNode = outNode.child("component").child("connections");
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_0"));
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_1"));
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_2"));
  doc.print(std::cout);
  delete[] childrenZero;
  delete[] childrenOne;
  delete[] childrenTwo;
  delete node;
}

BOOST_AUTO_TEST_CASE(xml_to_ainode_full) { // NOLINT(cert-err58-cpp)
  auto tgt = "assets/units/size_s/ship_arg_s_fighter_01.xml";
  auto ctx = TestUtil::GetTestContext(tgt);
  auto doc = TestUtil::GetXmlDocument(tgt);
  auto node = doc->root();
  auto componentNode = node.child("components").child("component");
  BOOST_TEST_REQUIRE(!componentNode.empty());
  auto component = Component(componentNode, ctx);

  auto result = component.ConvertToAiNode();

  BOOST_TEST_REQUIRE(result->mNumChildren > 0);
  TestUtil::checkAiNodeName(result->mChildren[0], "ship_arg_s_fighter_01");

  delete result;
}
//
// TODO layers? source? MUST DO class etc
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
