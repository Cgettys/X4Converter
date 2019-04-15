#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

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

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
BOOST_AUTO_TEST_SUITE(test_suite1) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(part) // NOLINT(cert-err58-cpp)


        BOOST_AUTO_TEST_CASE(test_read_part_name_correct) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());

            auto part = Part(partNode);
            BOOST_TEST(part.getName() == "anim_main");
            // TODO make sure nodes are not changed by reading
        }

        BOOST_AUTO_TEST_CASE(test_read_part_name_throws_on_empty) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            partNode.remove_attribute("name");

            BOOST_CHECK_THROW(auto part = Part(partNode),std::runtime_error);
        }

        BOOST_AUTO_TEST_CASE(test_write_part_name) { // NOLINT(cert-err58-cpp)
            const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
            pugi::xml_document expected;
            pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
            BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
            auto partNode = expected.select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            partNode.remove_attribute("name");

            BOOST_CHECK_THROW(auto part = Part(partNode),std::runtime_error);
        }


    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)