#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <boost/test/unit_test.hpp>

#include <X4ConverterTools/model/VisualLod.h>
#include <boost/algorithm/string.hpp>
#include "../testUtil.h"
namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests)

BOOST_AUTO_TEST_SUITE(LodUnitTests)
// TODO split into 2

BOOST_AUTO_TEST_CASE(read_lod_name) {
  std::string tgtPath = "assets/units/size_s/ship_arg_s_fighter_01";
  auto ctx = TestUtil::GetTestContext(tgtPath);
  auto lod = VisualLod(0, "anim_main", ctx);
  std::string expectedName = "anim_main-lod0";
  BOOST_TEST(lod.getName() == expectedName);
  auto result = lod.ConvertToAiNode();
  TestUtil::checkAiNodeName(result, expectedName, Lod::Qualifier);
  delete result;
}
// TODO rewrite/move to part
//BOOST_AUTO_TEST_CASE(read_lod_no_index) {
//  std::string tgtPath = "assets/units/size_s/ship_arg_s_fighter_01";
//  auto ctx = TestUtil::GetTestContext(tgtPath);
//  auto doc = TestUtil::GetXmlDocument(tgtPath + ".xml");
//  auto node = doc->select_node(
//      "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
//  BOOST_TEST_REQUIRE(!node.empty());
//  node.remove_attribute("index");
//
//  BOOST_CHECK_THROW(auto lod = VisualLod(node, "anim_main", ctx), std::runtime_error);
//}
//
//BOOST_AUTO_TEST_CASE(read_lod_wrong_type) { // NOLINT(cert-err58-cpp)
//  std::string tgtPath = "assets/units/size_s/ship_arg_s_fighter_01";
//  auto ctx = TestUtil::GetTestContext(tgtPath);
//  auto doc = TestUtil::GetXmlDocument(tgtPath + ".xml");
//  auto node = doc->select_node(
//      "/components/component/connections/connection[@name='Connection01']/parts/part").node();
//  BOOST_TEST_REQUIRE(!node.empty());
//  BOOST_CHECK_THROW(auto lod = VisualLod(node, "anim_main", ctx), std::runtime_error);
//}

BOOST_AUTO_TEST_CASE(lod_round_trip) { // NOLINT(cert-err58-cpp)
  std::string tgtPath = "assets/units/size_s/ship_arg_s_fighter_01";
  // TODO what do I even need the doc for here?
  auto ctx = TestUtil::GetTestContext(tgtPath);
  auto doc = TestUtil::GetXmlDocument(tgtPath + ".xml");
  aiScene scene;
  ctx->SetScene(&scene);
  auto node = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[@index='0']").node();
  BOOST_TEST_REQUIRE(!node.empty());

  auto lodForward = VisualLod(0, "anim_main", ctx);
  auto forwardResult = lodForward.ConvertToAiNode();
  ctx->PopulateSceneArrays();
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->SetScene(&scene);

  auto lodBackward = VisualLod(ctx2);
  auto outNode = doc->select_node(
      "/components/component/connections/connection[@name='Connection01']/parts/part/lods").node();
  outNode.remove_child("lod");
  outNode.remove_child("lod");
  outNode.remove_child("lod");
  lodBackward.ConvertFromAiNode(forwardResult);
  lodBackward.ConvertToGameFormat(outNode);
  outNode.print(std::cout);

  auto matsNode = outNode.find_child_by_attribute("lod", "index", "0").child("materials");
  BOOST_TEST_REQUIRE(matsNode);
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "1"));
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "2"));
  BOOST_TEST(matsNode.find_child_by_attribute("material", "id", "3"));
  BOOST_TEST_CHECKPOINT("Beginning cleanup");
  // Normally, scene would delete this but not this time since it is never added to the scene.
  delete forwardResult;
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
#pragma clang diagnostic pop