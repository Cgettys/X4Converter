#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/model/Connection.h>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(ConnectionUnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_CASE(xml_to_ainode_read_conn_offset) { // NOLINT(cert-err58-cpp)
  std::string tgtPath = "assets/units/size_s/ship_arg_s_fighter_01";
  auto ctx = TestUtil::GetTestContext(tgtPath);
  auto doc = TestUtil::GetXmlDocument(tgtPath + ".xml");
  auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
  node.remove_child("lods");
  BOOST_TEST_REQUIRE(!node.empty());

  auto conn = Connection(node, ctx);
  auto result = conn.ConvertToAiNode();
  aiMatrix4x4 expectedMatrix(aiVector3D(1, 1, 1), aiQuaternion(0.976296, -0, -0, -0.2164396),
                             aiVector3D(9.411734, -2.738604, -2.866085));
  BOOST_TEST(result->mTransformation.Equal(expectedMatrix));
  delete result;
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_write_conn_offset) {
  auto ctx = TestUtil::GetTestContext("TEST");
  auto node = new aiNode("Connection02");

  aiMatrix4x4 tmp(aiVector3D(1.0f, 1.0f, 1.0f),
                  aiQuaternion(0.976296f, -0.0f, -0.0f, -0.2164396f),
                  aiVector3D(9.411734f, -2.738604f, -2.866085f));
  node->mTransformation.a1 = tmp.a1;
  node->mTransformation.a2 = tmp.a2;
  node->mTransformation.a3 = tmp.a3;
  node->mTransformation.a4 = tmp.a4;
  node->mTransformation.b1 = tmp.b1;
  node->mTransformation.b2 = tmp.b2;
  node->mTransformation.b3 = tmp.b3;
  node->mTransformation.b4 = tmp.b4;
  node->mTransformation.c1 = tmp.c1;
  node->mTransformation.c2 = tmp.c2;
  node->mTransformation.c3 = tmp.c3;
  node->mTransformation.c4 = tmp.c4;
  node->mTransformation.d1 = tmp.d1;
  node->mTransformation.d2 = tmp.d2;
  node->mTransformation.d3 = tmp.d3;
  node->mTransformation.d4 = tmp.d4;

  auto conn = Connection(ctx);
  conn.ConvertFromAiNode(node);

  pugi::xml_document doc;
  auto outNode = doc.append_child("connections");
  conn.ConvertToGameFormat(outNode);

  auto offsetNode = outNode.child("connection").child("offset");
  auto posNode = offsetNode.child("position");
  auto quatNode = offsetNode.child("quaternion");

  // TODO make order of args right
  // TODO are we gonna bother flipping axes?
  BOOST_CHECK_EQUAL(posNode.attribute("x").as_float(), 9.411734f);
  BOOST_CHECK_EQUAL(posNode.attribute("y").as_float(), -2.738604f);
  BOOST_CHECK_EQUAL(posNode.attribute("z").as_float(), -2.866085f);
  // NB: ordering matching weird xml ordering
  aiQuaternion expectedQuat{0.976296f, -0.0f, -0.0f, -0.2164396f};
  auto actualQuat = util::XmlUtil::ReadAttrQuat(quatNode);
  TestUtil::checkQuaternion(quatNode.path(), expectedQuat, actualQuat);
  delete node;
}

BOOST_AUTO_TEST_CASE(from_xml_name) { // NOLINT(cert-err58-cpp)
  // TODO split into integration + unit
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto doc = TestUtil::GetXmlDocument("assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
  BOOST_TEST_REQUIRE(!node.empty());

  auto conn = Connection(node, ctx);
  BOOST_TEST(conn.getName() == "Connection02");
}

BOOST_AUTO_TEST_CASE(from_xml_no_parent) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto doc = TestUtil::GetXmlDocument("assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node("/components/component/connections/connection[@name='Connection01']").node();
  node.remove_child("lods");
  BOOST_TEST_REQUIRE(!node.empty());

  auto conn = Connection(node, ctx);
  BOOST_TEST(!conn.hasParent());
}

BOOST_AUTO_TEST_CASE(from_xml_has_parent) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto doc = TestUtil::GetXmlDocument("assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
  node.remove_child("lods");
  BOOST_TEST_REQUIRE(!node.empty());

  auto conn = Connection(node, ctx);
  BOOST_TEST(conn.hasParent());
  BOOST_TEST(conn.getParentName() == "anim_main");
}
BOOST_AUTO_TEST_CASE(from_ainode_name) { // NOLINT(cert-err58-cpp)
  auto ainode = new aiNode("*Connection02*");

  auto ctx = TestUtil::GetTestContext("TEST");

  auto conn = Connection(ainode, ctx);
  BOOST_TEST(conn.getName() == "*Connection02*");
  delete ainode;
}

BOOST_AUTO_TEST_CASE(xml_to_ainode_conn_attrs_tags) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
  auto doc = TestUtil::GetXmlDocument("assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
  node.remove_child("lods");
  BOOST_TEST_REQUIRE(!node.empty());

  auto conn = Connection(node, ctx);
  auto result = conn.ConvertToAiNode();
  BOOST_TEST(result->mNumChildren == 1);
  auto metadata = ctx->metadata->GetMetadata("Connection02");
  BOOST_TEST_REQUIRE(metadata.count("tags") == 1);
  BOOST_CHECK_EQUAL(metadata["tags"], "part animation iklink nocollision forceoutline detail_xl  ");
  // TODO handle parenting better
  BOOST_TEST_REQUIRE(metadata.count("parent") == 1);
  BOOST_CHECK_EQUAL(metadata["parent"], "anim_main");

  delete result;
}

BOOST_AUTO_TEST_CASE(ainode_to_xml_conn_attrs_tags) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext("TEST");
  auto node = new aiNode("[Cn]Connection02");
  std::string tagStr = "part animation iklink nocollision forceoutline detail_xl  ";
  ctx->metadata->SetAttribute("[Cn]Connection02", "tags", tagStr);
  pugi::xml_document doc;
  auto outNode = doc.append_child("connections");

  auto conn = Connection(node, ctx);
  conn.ConvertToGameFormat(outNode);

  auto connNode = outNode.find_child_by_attribute("connection", "name", "Connection02");
  BOOST_CHECK_EQUAL(std::string(connNode.attribute("tags").value()), tagStr);

  delete node;
}

// TODO animations, etc
// TODO ship_arg_s_fighter_01 restrictions
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
