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
  TestUtil::checkAiNodeName(result->mChildren[0], "ship_arg_s_fighter_01", Component::Qualifier);

  delete result;
}

BOOST_AUTO_TEST_CASE(from_ainode_basic) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto node = TestUtil::makeAiNode("ship_arg_s_fighter_01", Component::Qualifier);

  auto component = Component(ctx);
  component.ConvertFromAiNode(node);
  BOOST_TEST(component.getName() == "ship_arg_s_fighter_01");
  delete node;
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_complicated) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto shipName = "ship_arg_s_fighter_01";
  auto node = TestUtil::makeAiNode(shipName, Component::Qualifier);
  auto childrenZero = new aiNode *[2];
  childrenZero[0] = TestUtil::makeAiNode("test_conn_0", Connection::Qualifier);
  childrenZero[1] = TestUtil::makeAiNode("test_conn_1", Connection::Qualifier);
  auto childrenOne = new aiNode *[1];
  childrenOne[0] = TestUtil::makeAiNode("test_part_0", Part::Qualifier);
  auto childrenTwo = new aiNode *[1];
  childrenTwo[0] = TestUtil::makeAiNode("test_conn_2", Connection::Qualifier);
  childrenOne[0]->addChildren(1, childrenTwo);
  childrenZero[0]->addChildren(1, childrenOne);
  node->addChildren(2, childrenZero);
  pugi::xml_document doc;
  auto outNode = doc.append_child("components");


  // Set missing metadata:
  ctx->metadata->SetAttribute(shipName, "source", "don'tcare");

  auto component = Component(ctx);
  component.ConvertFromAiNode(node);
  BOOST_TEST_REQUIRE(component.getNumberOfConnections() == 3);

  component.ConvertToGameFormat(outNode);

  auto connsNode = outNode.child("component").child("connections");
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_0"));
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_1"));
  BOOST_TEST(connsNode.find_child_by_attribute("connection", "name", "test_conn_2"));
  // doc.print(std::cout);
  delete[] childrenZero;
  delete[] childrenOne;
  delete[] childrenTwo;
  delete node;
}

// TODO this is really an integration test
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
  TestUtil::checkAiNodeName(result->mChildren[0], "ship_arg_s_fighter_01", Component::Qualifier);

  delete result;
}
//
// TODO layers? source? MUST DO class etc
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
