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
#include <X4ConverterTools/model/Connection.h>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(ConnectionUnitTests) // NOLINT(cert-err58-cpp)

        BOOST_AUTO_TEST_CASE(xml_to_ainode_read_conn_offset) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node);
            auto result = conn.ConvertToAiNode();
            aiMatrix4x4 expectedMatrix(aiVector3D(1, 1, 1), aiQuaternion(0.976296, -0, -0, -0.2164396),
                                       aiVector3D(9.411734, -2.738604, -2.866085));
            BOOST_TEST(result->mTransformation.Equal(expectedMatrix));
            delete doc;
        }
        // TODO reverse

        BOOST_AUTO_TEST_CASE(from_xml_no_parent) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection01']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, "ship_arg_s_fighter_01");
            BOOST_TEST(conn.getParentName() == "ship_arg_s_fighter_01");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_xml_has_parent) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node);
            BOOST_TEST(conn.getParentName() == "anim_main");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(xml_to_ainode_conn_attrs_tags) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node);
            auto result = conn.ConvertToAiNode();
            BOOST_TEST_REQUIRE(result->mNumChildren == 2);
//        BOOST_TEST(result->mChildren[0]->mName); TODO someday
            TestUtil::checkAiNodeName(result->mChildren[1],
                                      "Connection02^tags^part animation iklink nocollision forceoutline detail_xl  ");
            delete doc;
        }

    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
// TODO animations, tags, etc
