#define BOOST_TEST_MODULE AllTests

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <assimp/types.h>
#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>
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
        const std::string gameBaseFolderPath = test::TestUtil::GetBasePath();
        const std::string tgtPath = "/assets/units/size_s/ship_gen_s_fighter_01";
        const std::string inputXMLPath = tgtPath + ".xml";
        const std::string daePath = tgtPath + ".out.dae";
        const std::string outputXMLPath = tgtPath + ".out.xml";
        // To prevent cross contamination between runs, remove dae to be safe
        fs::remove(gameBaseFolderPath + daePath);
        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
        fs::copy_file(gameBaseFolderPath + inputXMLPath, gameBaseFolderPath + outputXMLPath,
                      fs::copy_option::overwrite_if_exists);

        BOOST_TEST_CHECKPOINT("Begin test");
        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath, inputXMLPath, daePath);
        BOOST_TEST(forwardSuccess);
        BOOST_TEST_CHECKPOINT("Forward parsing");
        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath, daePath, outputXMLPath);
        BOOST_TEST(backwardSuccess);

        BOOST_TEST_CHECKPOINT("Backward parsing");
        auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
        auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
        TestUtil::CompareXMLFiles(expectedDoc, actualDoc);
        BOOST_TEST_CHECKPOINT("Cleanup");
        delete expectedDoc;
        delete actualDoc;

    }

    BOOST_AUTO_TEST_CASE(xml_hard) {
        // TODO refactor all the io...
        const std::string gameBaseFolderPath = test::TestUtil::GetBasePath();
        const std::string tgtPath = "/assets/units/size_s/ship_gen_s_fighter_01";
        const std::string inputXMLPath = tgtPath + ".xml";
        const std::string daePath = tgtPath + ".out.dae";
        const std::string outputXMLPath = tgtPath + ".out.xml";
        // To prevent cross contamination between runs, remove dae to be safe
        fs::remove(gameBaseFolderPath + daePath);
        // Also to prevent cross contamination, overwrite the output XML with something lacking connections.
        // TODO as we get further along, start cutting out more and more of this file
        pugi::xml_document doc;
        BOOST_TEST_REQUIRE(doc.load_file((gameBaseFolderPath + inputXMLPath).c_str()).status == pugi::status_ok);
        auto compNode = doc.child("components").child("component");
        compNode.remove_child("connections");
        BOOST_TEST_REQUIRE(doc.save_file((gameBaseFolderPath + outputXMLPath).c_str()));

        BOOST_TEST_CHECKPOINT("Begin test");
        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath, inputXMLPath, daePath);
        BOOST_TEST(forwardSuccess);
        BOOST_TEST_CHECKPOINT("Forward parsing");
        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath, daePath, outputXMLPath);
        BOOST_TEST(backwardSuccess);

        BOOST_TEST_CHECKPOINT("Backward parsing");
        auto expectedDoc = TestUtil::GetXmlDocument(inputXMLPath);
        auto actualDoc = TestUtil::GetXmlDocument(outputXMLPath);
        TestUtil::CompareXMLFiles(expectedDoc, actualDoc);
        BOOST_TEST_CHECKPOINT("Cleanup");
        delete expectedDoc;
        delete actualDoc;

    }

BOOST_AUTO_TEST_SUITE_END()

