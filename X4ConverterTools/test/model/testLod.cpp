#include <boost/test/unit_test.hpp>

#include <X4ConverterTools/model/VisualLod.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "../testUtil.h"
// NOLINT(cert-err58-cpp)
namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests)

BOOST_AUTO_TEST_SUITE(LodUnitTests)

BOOST_AUTO_TEST_CASE(read_lod_name) {
  auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
  BOOST_TEST_REQUIRE(!node.empty());

  auto lod = VisualLod(node, "anim_main", ctx);
  std::string expectedName = "anim_main-lod0";
  BOOST_TEST(lod.getName() == expectedName);
  auto result = lod.ConvertToAiNode(pugi::xml_node());
  BOOST_TEST(std::string(result->mName.C_Str()) == expectedName);
  delete doc;
  delete result;
}

BOOST_AUTO_TEST_CASE(read_lod_no_index) {
  auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
  BOOST_TEST_REQUIRE(!node.empty());
  node.remove_attribute("index");

  BOOST_CHECK_THROW(auto lod = VisualLod(node, "anim_main", ctx), std::runtime_error);
  delete doc;
}

BOOST_AUTO_TEST_CASE(read_lod_wrong_type) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
  BOOST_TEST_REQUIRE(!node.empty());
  BOOST_CHECK_THROW(auto lod = VisualLod(node, "anim_main", ctx), std::runtime_error);
  delete doc;
}

BOOST_AUTO_TEST_CASE(lod_round_trip) { // NOLINT(cert-err58-cpp)
  auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
  auto pScene = new aiScene();
  ctx->SetScene(pScene);
  auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto node = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[@index='0']").node();
  BOOST_TEST_REQUIRE(!node.empty());

  auto lodForward = VisualLod(node, "anim_main", ctx);
  auto forwardResult = lodForward.ConvertToAiNode(pugi::xml_node());
  ctx->PopulateSceneArrays();
  delete doc;
  ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
  ctx->SetScene(pScene);

  auto lodBackward = VisualLod(ctx);
  auto outDoc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
  auto outNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods").node();
  outNode.remove_child("lod");
  outNode.remove_child("lod");
  outNode.remove_child("lod");
  lodBackward.ConvertFromAiNode(forwardResult, pugi::xml_node());
  lodBackward.ConvertToGameFormat(outNode);

  auto matsNode = outNode.find_child_by_attribute("lod", "index", "0").child("materials");
  BOOST_TEST_REQUIRE(matsNode);
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "1"));
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "2"));
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "3"));
  delete outDoc;
}
// TODO fixme
//        BOOST_AUTO_TEST_CASE(part_name_from_ainode) {
//            auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
//            std::string lodName = "anim_main-lod0";
//            auto ainode = new aiNode(lodName);
//
//            auto lod = VisualLod(ctx);
//            lod.ConvertFromAiNode(ainode);
//            BOOST_TEST(lod.getName() == lodName);
//            BOOST_TEST(lod.getIndex() == 0);
//            delete ainode;
//        }
//
//        BOOST_AUTO_TEST_CASE(to_xml_one_lod) {
//            auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
//            std::string lodName = "anim_main-lod0";
//            auto ainode = new aiNode(lodName);
//
//            auto lod = VisualLod(ctx);
//            lod.ConvertFromAiNode(ainode);
//            pugi::xml_document doc;
//            auto lodsNode = doc.append_child("lods");
//            lod.ConvertToGameFormat(lodsNode);
//            BOOST_TEST(lodsNode.child("lod").empty() == false);
//            BOOST_TEST(lodsNode.child("lod").attribute("index").as_int() == 0);
//            delete ainode;
//        }
//
//        BOOST_AUTO_TEST_CASE(to_xml_two_lod) {
//            auto ctx = TestUtil::GetTestContext("assets\\units\\size_s\\ship_arg_s_fighter_01_data");
//            std::string lodZeroName = "anim_main-lod0";
//            auto ainodeZero = new aiNode(lodZeroName);
//            auto lodZero = VisualLod(ctx);
//            lodZero.ConvertFromAiNode(ainodeZero);
//
//            std::string lodOneName = "anim_main-lod1";
//            auto ainodeOne = new aiNode(lodOneName);
//            auto lodOne = VisualLod(ctx);
//            lodOne.ConvertFromAiNode(ainodeOne);
//            pugi::xml_document doc;
//            auto lodsNode = doc.append_child("lods");
//            lodZero.ConvertToGameFormat(lodsNode);
//            lodOne.ConvertToGameFormat(lodsNode);
//
//            auto firstChild = lodsNode.first_child();
//            auto secondChild = lodsNode.last_child();
//            BOOST_TEST(std::string(firstChild.name()) == "lod");
//            BOOST_TEST(std::string(secondChild.name()) == "lod");
//            BOOST_TEST(firstChild.attribute("index").as_int() == 0);
//            BOOST_TEST(secondChild.attribute("index").as_int() == 1);
//            delete ainodeZero;
//            delete ainodeOne;
//        }
// TODO overwrite lod case

// TODO failure case

// TODO materials
// TODO check that name makes sense with parent name
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()