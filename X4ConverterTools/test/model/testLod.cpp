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
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(LodUnitTests) // NOLINT(cert-err58-cpp)


        BOOST_AUTO_TEST_CASE(read_lod_name) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument( "/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto lod = Lod(node, "anim_main");
            std::string expectedName = "anim_main^lod0";
            BOOST_TEST(lod.getName() == expectedName);
            auto result = lod.ConvertToAiNode();
            BOOST_TEST(std::string(result->mName.C_Str()) == expectedName);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(read_lod_no_index) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument( "/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part/lods/lod[1]").node();
            BOOST_TEST_REQUIRE(!node.empty());
            node.remove_attribute("index");

            BOOST_CHECK_THROW(auto lod = Lod(node, "anim_main"), std::runtime_error);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(read_lod_wrong_type) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument( "/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!node.empty());
            node.remove_attribute("index");

            BOOST_CHECK_THROW(auto lod = Lod(node, "anim_main"), std::runtime_error);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(part_name_from_ainode) { // NOLINT(cert-err58-cpp)
            std::string lodName = "anim_main^lod0";
            auto ainode = new aiNode(lodName);

            auto lod = Lod();
            lod.ConvertFromAiNode(ainode);
            BOOST_TEST(lod.getName() == lodName);
            BOOST_TEST(lod.getIndex() == 0);
        }
        // TODO failure case

        // TODO materials
// TODO check that name makes sense with parent name
// TODO in part make assertions about lods existing
    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)