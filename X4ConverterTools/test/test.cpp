#define BOOST_TEST_MODULE AllTests

#include "pugixml.hpp"
#include <set>
#include <algorithm>
#include <vector>
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

namespace fs = boost::filesystem;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(test_suite1)
    BOOST_AUTO_TEST_CASE(test_ojump_xr_file) {        // TODO refactor all the io...
        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
        const std::string tgtPath = "extensions/DOR_XR_ORIGINAL/assets/units/size_xl/units_size_xl_red_destroyer";
        const std::string inputXMLPath = gameBaseFolderPath + tgtPath + ".xml";
        const std::string daePath = gameBaseFolderPath + tgtPath + ".out.dae";
        const std::string outputXMLPath = gameBaseFolderPath + tgtPath + ".out.xml";
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
        //https://pugixml.org/docs/manual.html#access.walker
        // See also: https://stackoverflow.com/a/29599648
        struct my_walker : pugi::xml_tree_walker {
            std::set<std::string> paths;

            virtual bool for_each(pugi::xml_node &node) {
                std::string path = node.path();
//			std::cout << path << std::endl;
                // Duplicate element == BAD
//			BOOST_TEST(!paths.count(path));
                paths.insert(path);
                return true; // continue traversal
            }
        };
        my_walker expectedWalk;
        expectedDoc.traverse(expectedWalk);
        my_walker actualWalk;
        actualDoc.traverse(actualWalk);
        std::set<std::string> expectedMinusActual;
        std::set_difference(expectedWalk.paths.begin(), expectedWalk.paths.end(), actualWalk.paths.begin(),
                            actualWalk.paths.end(), std::inserter(expectedMinusActual, expectedMinusActual.begin()));

        std::set<std::string> actualMinusExpected;
        std::set_difference(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                            expectedWalk.paths.end(), std::inserter(actualMinusExpected, actualMinusExpected.begin()));

        std::set<std::string> intersection;
        std::set_intersection(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                              expectedWalk.paths.end(), std::inserter(intersection, intersection.begin()));
        BOOST_TEST(expectedMinusActual.size() == 0);
        for (auto &&x : expectedMinusActual) {
            printf("Output was missing path: %s\n", x.c_str());
        }
        BOOST_TEST(actualMinusExpected.size() == 0);
        for (auto &&x : actualMinusExpected) {
            printf("Output has extra path: %s\n", x.c_str());
        }
        // TODO more validation
        BOOST_TEST_CHECKPOINT("Cleanup");
    }
    BOOST_AUTO_TEST_CASE(test_ojump_file) {
        const std::string gameBaseFolderPath = "/home/cg/Desktop/X4/unpacked/";
        const std::string tgtPath = "extensions/DOR/assets/units/size_xl/units_size_xl_cargo_hauler_2";
        const std::string inputXMLPath = gameBaseFolderPath +tgtPath + ".xml";
        const std::string daePath = gameBaseFolderPath +tgtPath+ "_modified.dae";
        const std::string outputXMLPath = gameBaseFolderPath +tgtPath +"_modified.xml";
        const std::string daeOutPath = gameBaseFolderPath +tgtPath+ "_modified.out.dae";
        // To prevent cross contamination between runs, remove dae to be safe
        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
        fs::copy_file(inputXMLPath, outputXMLPath, fs::copy_option::overwrite_if_exists);

        BOOST_TEST_CHECKPOINT("Backward parsing");
        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath.c_str(), daePath.c_str(), outputXMLPath.c_str());
        BOOST_TEST(backwardSuccess);

        // TODO why did this start seg-faulting?
//        BOOST_TEST_CHECKPOINT("Forward parsing");
//        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath.c_str(), outputXMLPath.c_str(),daeOutPath.c_str());
//        BOOST_TEST(forwardSuccess);


}

    BOOST_AUTO_TEST_CASE(test_xml) {
        // TODO refactor all the io...
        // TODO par scout not quite right
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
        const std::string tgtPath = "assets/props/SurfaceElements/shield_arg_l_standard_01_mk2";
//        const std::string tgtPath = "assets/units/size_s/ship_gen_s_fighter_01";
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
        //https://pugixml.org/docs/manual.html#access.walker
        // See also: https://stackoverflow.com/a/29599648
        struct my_walker : pugi::xml_tree_walker {
            std::set<std::string> paths;

            virtual bool for_each(pugi::xml_node &node) {
                std::string path = node.path();
//			std::cout << path << std::endl;
                // Duplicate element == BAD
//			BOOST_TEST(!paths.count(path));
                paths.insert(path);
                return true; // continue traversal
            }
        };
        my_walker expectedWalk;
        expectedDoc.traverse(expectedWalk);
        my_walker actualWalk;
        actualDoc.traverse(actualWalk);
        std::set<std::string> expectedMinusActual;
        std::set_difference(expectedWalk.paths.begin(), expectedWalk.paths.end(), actualWalk.paths.begin(),
                            actualWalk.paths.end(), std::inserter(expectedMinusActual, expectedMinusActual.begin()));

        std::set<std::string> actualMinusExpected;
        std::set_difference(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                            expectedWalk.paths.end(), std::inserter(actualMinusExpected, actualMinusExpected.begin()));

        std::set<std::string> intersection;
        std::set_intersection(actualWalk.paths.begin(), actualWalk.paths.end(), expectedWalk.paths.begin(),
                              expectedWalk.paths.end(), std::inserter(intersection, intersection.begin()));
        BOOST_TEST(expectedMinusActual.size() == 0);
        for (auto &&x : expectedMinusActual) {
            printf("Output was missing path: %s\n", x.c_str());
        }
        BOOST_TEST(actualMinusExpected.size() == 0);
        for (auto &&x : actualMinusExpected) {
            printf("Output has extra path: %s\n", x.c_str());
        }
        // TODO more validation


        BOOST_TEST_CHECKPOINT("Cleanup");


    }

BOOST_AUTO_TEST_SUITE_END()

