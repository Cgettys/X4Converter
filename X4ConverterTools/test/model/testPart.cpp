#include <boost/test/unit_test.hpp>


#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
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
            BOOST_TEST(std::string(result->mName.C_Str()) == "*anim_main*");
            delete doc;
            delete result;
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
            std::string name = part.getName();
            BOOST_TEST(name == partName);
            delete ainode;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_name) { // NOLINT(cert-err58-cpp)
            std::string partName = "testpart";
            auto ainode = new aiNode(partName);

            auto part = Part();
            part.ConvertFromAiNode(ainode);
            pugi::xml_document doc;
            auto node = doc.append_child("parts");
            part.ConvertToXml(node);

            std::string actualName = node.child("part").attribute("name").value();
            BOOST_TEST(partName == actualName);

            delete ainode;
        }

        BOOST_AUTO_TEST_CASE(xml_to_ainode_read_ref) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection35']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            auto part = Part(partNode);

            auto result = part.ConvertToAiNode();
            TestUtil::checkAiNodeName(result, "*anim_thruster_06*");
            BOOST_TEST_REQUIRE(result->mNumChildren == 1);
            TestUtil::checkAiNodeName(result->mChildren[0], "anim_thruster_06|DO_NOT_EDIT.ref|thruster_ship_s_01.anim_thruster_001");
            delete doc;
            delete result;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_write_ref) { // NOLINT(cert-err58-cpp)
            std::string partName = "anim_thruster_06";
            std::string childName = "anim_thruster_06|DO_NOT_EDIT.ref|thruster_ship_s_01.anim_thruster_001";
            auto node = new aiNode(partName);
            auto children = new aiNode *[1];
            children[0] = new aiNode(childName);
            node->addChildren(1, children);

            auto part = Part();
            part.ConvertFromAiNode(node);

            pugi::xml_document doc;
            auto outNode = doc.append_child("parts");
            part.ConvertToXml(outNode);

            auto partNode = outNode.child("part");
            std::string ref = partNode.attribute("ref").value();
            BOOST_TEST("thruster_ship_s_01.anim_thruster_001" == ref);
            delete node;
            delete[] children;
        }



//        BOOST_AUTO_TEST_CASE(ainode_to_xml_to_read_ref) { // NOLINT(cert-err58-cpp)
//            auto node = new aiNode("Connection35");
//        }
        // TODO global naming constraints, wrecks, uv_animation
        // TODO does a collision mesh always exist?
        // TODO size, sizeraw,pivot, sounds, effectemmiters etc

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(IntegrationTests) // NOLINT(cert-err58-cpp)
    BOOST_AUTO_TEST_SUITE(PartIntegrationTests) // NOLINT(cert-err58-cpp)
        BOOST_AUTO_TEST_CASE(xml_to_ainode_lods) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto partNode = doc->select_node(
                    "/components/component/connections/connection[@name='Connection01']/parts/part").node();
            BOOST_TEST_REQUIRE(!partNode.empty());
            auto part = Part(partNode);

            auto result = part.ConvertToAiNode();
            BOOST_TEST(std::string(result->mName.C_Str()) == "anim_main");
            BOOST_TEST_REQUIRE(result->mNumChildren == 5);
            TestUtil::checkAiNodeName(result->mChildren[0], "anim_main|collision");
            TestUtil::checkAiNodeName(result->mChildren[1], "anim_main|lod0");
            TestUtil::checkAiNodeName(result->mChildren[2], "anim_main|lod1");
            TestUtil::checkAiNodeName(result->mChildren[3], "anim_main|lod2");
            TestUtil::checkAiNodeName(result->mChildren[4], "anim_main|lod3");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_lods) { // NOLINT(cert-err58-cpp)
            std::string partName = "testpart";
            auto ainode = new aiNode(partName);
            auto ainodeChildren = new aiNode *[3];
            ainodeChildren[0] = new aiNode(partName + "|collision");
            ainodeChildren[1] = new aiNode(partName + "|lod1");
            ainodeChildren[2] = new aiNode(partName + "|lod2");
            ainode->addChildren(3, ainodeChildren);

            auto part = Part();
            part.ConvertFromAiNode(ainode);
            pugi::xml_document doc;
            auto node = doc.append_child("parts");
            part.ConvertToXml(node);

            BOOST_TEST(!node.child("part").child("lods").empty());
        }
    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)