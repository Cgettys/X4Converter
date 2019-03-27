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
#include <X4ConverterTools/API.h>
#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Xmf/XuMeshFile.h>

namespace fs = boost::filesystem;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(test_suite1)


    BOOST_AUTO_TEST_CASE(test_xml) {
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
        const std::string inputXMLPath = gameBaseFolderPath
                                         + "assets/units/size_s/ship_gen_s_fighter_01.xml";
        const std::string daePath =
                gameBaseFolderPath
                + "assets/units/size_s/ship_gen_s_fighter_01.out.dae";
        const std::string outputXMLPath =
                gameBaseFolderPath
                + "assets/units/size_s/ship_gen_s_fighter_01.out.xml";
        // To prevent cross contamination between runs, remove dae to be safe
        fs::remove(daePath);
        // Also to prevent cross contamination, overwrite the output XML with original copy. Converter expects to be working on original; this lets us compare it to that
        fs::copy_file(inputXMLPath, outputXMLPath, fs::copy_option::overwrite_if_exists);

        BOOST_TEST_CHECKPOINT("Begin test");
        char szError[256];
        bool forwardSuccess = ConvertXmlToDae(gameBaseFolderPath.c_str(),
                                              inputXMLPath.c_str(), daePath.c_str(), szError, sizeof(szError));
        BOOST_TEST(forwardSuccess);
        BOOST_TEST_CHECKPOINT("Forward parsing");
        bool backwardSuccess = ConvertDaeToXml(gameBaseFolderPath.c_str(),
                                               daePath.c_str(), outputXMLPath.c_str(), szError, sizeof(szError));
        BOOST_TEST(backwardSuccess);

        BOOST_TEST_CHECKPOINT("Backward parsing");
        pugi::xml_document expectedDoc;
        pugi::xml_parse_result expectedResult = expectedDoc.load_file(
                inputXMLPath.c_str());

        pugi::xml_document actualDoc;
        pugi::xml_parse_result actualResult = actualDoc.load_file(
                outputXMLPath.c_str());
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
        std::set_difference(expectedWalk.paths.begin(), expectedWalk.paths.end(),
                            actualWalk.paths.begin(), actualWalk.paths.end(),
                            std::inserter(expectedMinusActual, expectedMinusActual.begin()));

        std::set<std::string> actualMinusExpected;
        std::set_difference(actualWalk.paths.begin(), actualWalk.paths.end(),
                            expectedWalk.paths.begin(), expectedWalk.paths.end(),
                            std::inserter(actualMinusExpected, actualMinusExpected.begin()));

        std::set<std::string> intersection;
        std::set_intersection(actualWalk.paths.begin(), actualWalk.paths.end(),
                              expectedWalk.paths.begin(), expectedWalk.paths.end(),
                              std::inserter(intersection, intersection.begin()));
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

