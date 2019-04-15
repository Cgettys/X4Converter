#include <boost/test/unit_test.hpp>


#include <X4ConverterTools/model/Lod.h>
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
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(LodUnitTests) // NOLINT(cert-err58-cpp)


    BOOST_AUTO_TEST_CASE(test_read_lod_name) { // NOLINT(cert-err58-cpp)
        const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
        pugi::xml_document expected;
        pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
        BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
        auto node = expected.select_node(
                "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
        BOOST_TEST_REQUIRE(!node.empty());

        auto lod = Lod(node,"anim_main");
        std::string expectedName = "anim_main^lod0";
        BOOST_TEST(lod.getName() == expectedName);
        auto result = lod.ConvertToAiNode();
        BOOST_TEST(std::string(result->mName.C_Str()) == expectedName);
    }

    BOOST_AUTO_TEST_CASE(test_read_lod_no_index) { // NOLINT(cert-err58-cpp)
        const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
        pugi::xml_document expected;
        pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
        BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
        auto node = expected.select_node(
                "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
        BOOST_TEST_REQUIRE(!node.empty());
        node.remove_attribute("index");

        BOOST_CHECK_THROW(auto lod = Lod(node,"anim_main"), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(test_read_lod_wrong_type) { // NOLINT(cert-err58-cpp)
        const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_arg_s_fighter_01.xml";
        pugi::xml_document expected;
        pugi::xml_parse_result expectedResult = expected.load_file(xmlFile.c_str());
        BOOST_TEST_REQUIRE(expectedResult.status == pugi::status_ok);
        auto node = expected.select_node(
                "/components/component/connections/connection[@name='Connection01']/parts/part").node();
        BOOST_TEST_REQUIRE(!node.empty());
        node.remove_attribute("index");

        BOOST_CHECK_THROW(auto lod = Lod(node,"anim_main"), std::runtime_error);
    }



// TODO in part make assertions about lods existing
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)