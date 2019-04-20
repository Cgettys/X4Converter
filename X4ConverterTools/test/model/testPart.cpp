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
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(PartUnitTests) // NOLINT(cert-err58-cpp)

        BOOST_AUTO_TEST_CASE(from_xml_read_part_name_correct) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());

            auto part = Part(partNode);
            BOOST_TEST(part.getName() =="anim_main");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(xml_to_ainode_read_part_name_correct) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());

            auto part = Part(partNode);
            auto result = part.ConvertToAiNode();
            BOOST_TEST(std::string(result->mName.C_Str()) == "anim_main");
            // TODO make sure nodes are not changed by reading
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_xml_read_part_name_throws_on_empty) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            partNode.remove_attribute("name");

            BOOST_CHECK_THROW(auto part = Part(partNode), std::runtime_error);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_xml_name_throws_on_wrong_type) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts[1]").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            BOOST_CHECK_THROW(auto part = Part(partNode), std::runtime_error);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_ai_node_part_name) { // NOLINT(cert-err58-cpp)
            std::string partName = "testpart";
            auto ainode = new aiNode(partName);

            auto part = Part();
            part.ConvertFromAiNode(ainode);
            BOOST_TEST(part.getName() == partName);
        }


        BOOST_AUTO_TEST_CASE(xml_to_ainode_read_ref) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection35']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            auto part = Part(partNode);

            auto result = part.ConvertToAiNode();
            BOOST_TEST(std::string(result->mName.C_Str()) == "anim_thruster_06");
            BOOST_TEST_REQUIRE(result->mNumChildren == 1);
            TestUtil::checkAiNodeName(result->mChildren[0], "DO_NOT_EDIT^ref^thruster_ship_s_01.anim_thruster_001");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_to_read_ref) { // NOLINT(cert-err58-cpp)
            auto node = new aiNode("Connection35");
        }
        // TODO global naming constraints
        // TODO wrecks, uv_animation
        // TODO does a collision mesh always exist?
        // TODO size, sizeraw,pivot, sounds, effectemmiters etc

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(IntegrationTests) // NOLINT(cert-err58-cpp)
    BOOST_AUTO_TEST_SUITE(PartIntegrationTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)