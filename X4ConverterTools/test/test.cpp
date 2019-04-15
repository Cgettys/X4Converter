#define BOOST_TEST_MODULE AllTests

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>
#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/types.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>

#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include "testUtil.h"

namespace fs = boost::filesystem;
using namespace Assimp;
using namespace test;
BOOST_AUTO_TEST_SUITE(test_suite1)
// TODO tests for changing stuff
//    BOOST_AUTO_TEST_CASE(test_ojump_xr_file) {        // TODO refactor all the io...
//        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
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
//    BOOST_AUTO_TEST_CASE(test_ojump_file) {
//        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
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

    BOOST_AUTO_TEST_CASE(test_xml) {
    // TODO from nothing test case
        // TODO refactor all the io...
//        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
//        const std::string inputXMLPath = gameBaseFolderPath
//                                         + "extensions/break/assets/units/size_s/ship_gen_s_fighter_02.xml";
//        const std::string daePath =
//                gameBaseFolderPath
//                + "extensions/break/assets/units/size_s/ship_gen_s_fighter_02.out.dae";
//        const std::string outputXMLPath =
//                gameBaseFolderPath
//                + "extensions/break/assets/units/size_s/ship_gen_s_fighter_02.out.xml";
        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
//        const std::string tgtPath = "assets/units/size_s/ship_arg_s_scout_01";
//        const std::string tgtPath = "assets/units/size_m/ship_tel_m_frigate_01";
//        const std::string tgtPath = "assets/props/SurfaceElements/shield_arg_l_standard_01_mk2";
        const std::string tgtPath = "assets/units/size_s/ship_gen_s_fighter_01";
//        const std::string tgtPath = "assets/units/size_s/ship_par_s_scout_01";

//        const std::string tgtPath = "assets/units/size_m/ship_par_m_corvette_01";
        const std::string inputXMLPath = gameBaseFolderPath +tgtPath + ".xml";
        const std::string daePath = gameBaseFolderPath +tgtPath+ ".out.dae";
        const std::string outputXMLPath = gameBaseFolderPath +tgtPath +".out.xml";
        // To prevent cross contamination between runs, remove dae to be safe
        fs::remove(daePath);
        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
        fs::copy_file(inputXMLPath, outputXMLPath, fs::copy_option::overwrite_if_exists);

        BOOST_TEST_CHECKPOINT("Begin test");
        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath.c_str(), inputXMLPath.c_str(), daePath.c_str());
        BOOST_TEST(forwardSuccess);
        BOOST_TEST_CHECKPOINT("Forward parsing");
        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath.c_str(), daePath.c_str(), outputXMLPath.c_str());
        BOOST_TEST(backwardSuccess);

        BOOST_TEST_CHECKPOINT("Backward parsing");
        pugi::xml_document expectedDoc;
        pugi::xml_parse_result expectedResult = expectedDoc.load_file(inputXMLPath.c_str());

        pugi::xml_document actualDoc;
        pugi::xml_parse_result actualResult = actualDoc.load_file(outputXMLPath.c_str());

        TestUtil::CompareXMLFiles(expectedDoc, actualDoc);
        BOOST_TEST_CHECKPOINT("Cleanup");


    }

BOOST_AUTO_TEST_SUITE_END()

