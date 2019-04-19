#include <boost/test/unit_test.hpp>


#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <assimp/scene.h>
#include <pugixml.hpp>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(PartUnitTests) // NOLINT(cert-err58-cpp)


        BOOST_AUTO_TEST_CASE(test_read_part_name_correct) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile =test::TestUtil::GetBasePath()+"/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());

            auto part = Part(partNode);
            std::string expectedName = "anim_main";
            BOOST_TEST(part.getName() == expectedName);
            auto result = part.ConvertToAiNode();
            BOOST_TEST(std::string(result->mName.C_Str()) == expectedName);
            // TODO make sure nodes are not changed by reading
        }

        BOOST_AUTO_TEST_CASE(test_read_part_name_throws_on_empty) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile =test::TestUtil::GetBasePath()+"/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            partNode.remove_attribute("name");

            BOOST_CHECK_THROW(auto part = Part(partNode), std::runtime_error);
        }

        BOOST_AUTO_TEST_CASE(test_read_part_name_throws_on_wrong_type) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile = test::TestUtil::GetBasePath()+"/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts[1]").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            BOOST_CHECK_THROW(auto part = Part(partNode), std::runtime_error);
        }

        BOOST_AUTO_TEST_CASE(test_part_name_from_ainode) { // NOLINT(cert-err58-cpp)
            std::string partName = "testpart";
            auto ainode = new aiNode(partName);

            auto part = Part();
            part.ConvertFromAiNode(ainode);
            BOOST_TEST(part.getName() == partName);
        }



        // TODO global naming constraints
        // TODO wrecks
        // TODO does a collision mesh always exist?
        // TODO size, sizeraw,pivot, sounds, effectemmiters etc

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(IntegrationTests) // NOLINT(cert-err58-cpp)
    BOOST_AUTO_TEST_SUITE(PartIntegrationTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)