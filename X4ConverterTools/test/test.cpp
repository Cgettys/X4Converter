#define BOOST_TEST_MODULE AllTests

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <assimp/types.h>
#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/xmf/XmfFile.h>
#include <X4ConverterTools/model/Connection.h>

#include "testUtil.h"

namespace fs = boost::filesystem;
using namespace Assimp;
using namespace test;
BOOST_AUTO_TEST_SUITE(IntegrationTests)
// TODO tests for changing stuff
//    BOOST_AUTO_TEST_CASE(ojump_xr_file) {        // TODO refactor all the io...
//        const std::string gameBaseFolderPath = test::TestUtil::GetBasePath()+"/";
//        const std::string tgtPath = "extensions/DOR_XR_ORIGINAL/assets/units/size_xl/units_size_xl_red_destroyer";
//        const std::string inputXMLPath = gameBaseFolderPath + tgtPath + ".xml";
//        const std::string daePath = gameBaseFolderPath + tgtPath + ".out.dae";
//        const std::string outputXMLPath = gameBaseFolderPath + tgtPath + ".out.xml";
//        // To prevent cross contamination between runs, remove dae to be safe
//        fs::remove(daePath);
//        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
//        fs::copy_file(inputXMLPath, outputXMLPath, fs::copy_option::overwrite_if_exists);
//
//        BOOST_TEST_CHECKPOINT("Begin test");
//        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath.c_str(), inputXMLPath.c_str(), daePath.c_str());
//        BOOST_TEST(forwardSuccess);
//        BOOST_TEST_CHECKPOINT("Forward parsing");
//        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath.c_str(), daePath.c_str(), outputXMLPath.c_str());
//        BOOST_TEST(backwardSuccess);
//
//        BOOST_TEST_CHECKPOINT("Backward parsing");
//        pugi::xml_document expectedDoc;
//        pugi::xml_parse_result expectedResult = expectedDoc.load_file(inputXMLPath.c_str());
//
//        pugi::xml_document actualDoc;
//        pugi::xml_parse_result actualResult = actualDoc.load_file(outputXMLPath.c_str());
//    CompareXMLFiles(expectedDoc, actualDoc);
//        BOOST_TEST_CHECKPOINT("Cleanup");
//    }
// TODO fixme
//    BOOST_AUTO_TEST_CASE(ojump_file) {
//        const std::string gameBaseFolderPath = test::TestUtil::GetBasePath()+"/";
//        const std::string tgtPath = "extensions/DOR/assets/units/size_xl/units_size_xl_cargo_hauler_2";
//        const std::string inputXMLPath = gameBaseFolderPath +tgtPath + ".xml";
//        const std::string daePath = gameBaseFolderPath +tgtPath+ "_modified.dae";
//        const std::string outputXMLPath = gameBaseFolderPath +tgtPath +"_modified.xml";
//        const std::string daeOutPath = gameBaseFolderPath +tgtPath+ "_modified.out.dae";
//        // To prevent cross contamination between runs, remove dae to be safe
//        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
//        fs::copy_file(inputXMLPath, outputXMLPath, fs::copy_option::overwrite_if_exists);
//
//        BOOST_TEST_CHECKPOINT("Backward parsing");
//        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath.c_str(), daePath.c_str(), outputXMLPath.c_str());
//        BOOST_TEST(backwardSuccess);
//
////        // TODO why did this start seg-faulting?
//        BOOST_TEST_CHECKPOINT("Forward parsing");
//        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath.c_str(), outputXMLPath.c_str(),daeOutPath.c_str());
//        BOOST_TEST(forwardSuccess);
//
//
//}


BOOST_AUTO_TEST_CASE(xml) {
  // TODO refactor all the io...
  auto base = test::TestUtil::GetBasePath();
  const fs::path tgtPath = "assets/units/size_s/ship_gen_s_fighter_01";
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
  fs::remove(base / outputXMLPath);

  auto ctx = TestUtil::GetTestContext(tgtPath);
  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  BOOST_TEST_CHECKPOINT("Forward parsing");

  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto layer = expectedDoc->select_node("//components/component/layers/layer");
  //layer.node().remove_child("lights");
  layer.node().remove_child("sounds");
  layer.node().remove_child("trailemitters");

  auto animations = expectedDoc->select_nodes("//components/component/connections/connection");
  for (auto &&x : animations) {
    x.node().remove_child("animations");
  }
  auto parts = expectedDoc->select_nodes("//components/component/connections/connection/parts/part");
  for (auto &&x : parts) {
    x.node().remove_child("size_raw");
    // TODO FIXME!!
    x.node().remove_child("size");
  }

  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  // Such a hack but oh well TODO fixme
  auto actualParts = actualDoc->select_nodes("//components/component/connections/connection/parts/part");
  for (auto &&x : actualParts) {
    x.node().remove_child("size_raw");
    // TODO FIXME!!
    x.node().remove_child("size");
  }
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());
}
BOOST_AUTO_TEST_CASE(xml_medium) {
  auto base = test::TestUtil::GetBasePath();
  // TODO refactor all the io...
  // TODO test /assets/units/size_m/ship_arg_m_bomber_02.xml as well
  const fs::path tgtPath = "assets/units/size_s/ship_gen_s_fighter_01";
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // TODO absolute path handling tests
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  fs::remove(base / outputXMLPath);
  // Also to prevent cross contamination, overwrite the output XML with something lacking connections.
  pugi::xml_document doc;
  auto ctx = TestUtil::GetTestContext(tgtPath);

  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  BOOST_TEST_CHECKPOINT("Forward parsing");
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto compNode = expectedDoc->select_node("//components/component[@name='ship_gen_s_fighter_01']").node();
  // TODO as we get further along, leave in more of this
  //compNode.remove_child("layers");

  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());
}
BOOST_AUTO_TEST_CASE(xml_hard) {
  auto base = test::TestUtil::GetBasePath();
  // TODO refactor all the io...
  // TODO test /assets/units/size_m/ship_arg_m_bomber_02.xml as well
  const fs::path tgtPath = "assets/units/size_s/ship_gen_s_fighter_01";
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // TODO absolute path handling tests
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  fs::remove(base / outputXMLPath);
  // Also to prevent cross contamination, overwrite the output XML with something lacking connections.
  pugi::xml_document doc;
  auto ctx = TestUtil::GetTestContext(tgtPath);

  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  BOOST_TEST_CHECKPOINT("Forward parsing");
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto compNode = expectedDoc->select_node("//components/component[@name='ship_gen_s_fighter_01']").node();
  // TODO as we get further along, leave in more of this
  //compNode.remove_child("layers");

  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());
}

BOOST_AUTO_TEST_CASE(xml_hard_2) {
  // TODO no more io
  auto base = test::TestUtil::GetBasePath();
  const fs::path tgtPath = "assets/units/size_m/ship_arg_m_bomber_02";
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  fs::remove(base / outputXMLPath);
  // Also to prevent cross contamination, overwrite the output XML with something lacking connections.
  pugi::xml_document doc;
  auto ctx = TestUtil::GetTestContext(tgtPath);

  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  BOOST_TEST_CHECKPOINT("Forward parsing");
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto compNode = expectedDoc->select_node("//components/component[@name='ship_gen_s_fighter_01']").node();

  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());
}
BOOST_AUTO_TEST_CASE(bridge) {
  // TODO refactor all the io...
  auto base = test::TestUtil::GetBasePath();
  const fs::path tgtPath = "assets/interiors/bridges/bridge_arg_xl_01";
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
  fs::copy_file(base / inputXMLPath, base / outputXMLPath, fs::copy_option::overwrite_if_exists);
  auto ctx = TestUtil::GetTestContext(tgtPath);

  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  BOOST_TEST_CHECKPOINT("Forward parsing");
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());

}

BOOST_AUTO_TEST_CASE(multimat) {
  // TODO refactor all the io...
  auto base = test::TestUtil::GetBasePath();
  const fs::path tgtPath = "assets/units/size_m/ship_arg_m_trans_container_01";
  // TODO lowercase hatch name in code instead of the hack
  auto inputXMLPath = tgtPath.generic_path().replace_extension(".xml").string();
  auto daePath = tgtPath.generic_path().replace_extension(".out.dae").string();
  auto outputXMLPath = tgtPath.generic_path().replace_extension(".out.xml").string();
  // To prevent cross contamination between runs, remove dae to be safe
  fs::remove(base / daePath);
  // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
  fs::copy_file(base / inputXMLPath, base / outputXMLPath, fs::copy_option::overwrite_if_exists);
  auto ctx = TestUtil::GetTestContext(tgtPath);

  BOOST_TEST_CHECKPOINT("Begin test");
  bool forwardSuccess = ConvertXmlToDae(ctx, inputXMLPath, daePath);
  BOOST_TEST(forwardSuccess);
  BOOST_TEST_CHECKPOINT("Forward parsing");
  auto ctx2 = TestUtil::GetTestContext(tgtPath);
  ctx2->metadata->SetAll(ctx->metadata->GetAll());// TODO replace me
  bool backwardSuccess = ConvertDaeToXml(ctx2, daePath, outputXMLPath);
  BOOST_TEST(backwardSuccess);

  BOOST_TEST_CHECKPOINT("Backward parsing");
  auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
  auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
  TestUtil::CompareXMLFiles(expectedDoc.get(), actualDoc.get());

}
BOOST_AUTO_TEST_SUITE_END()

