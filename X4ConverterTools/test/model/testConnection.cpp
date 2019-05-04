#include <boost/test/unit_test.hpp>


#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
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

            ConversionContext ctx(TestUtil::GetBasePath());
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, ctx);
            auto result = conn.ConvertToAiNode(ctx);
            aiMatrix4x4 expectedMatrix(aiVector3D(1, 1, 1), aiQuaternion(0.976296, -0, -0, -0.2164396),
                                       aiVector3D(9.411734, -2.738604, -2.866085));
            BOOST_TEST(result->mTransformation.Equal(expectedMatrix));
            delete doc;
            delete result;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_write_conn_offset) {

            ConversionContext ctx(TestUtil::GetBasePath());
            auto node = new aiNode("Connection02");

            aiMatrix4x4 tmp(aiVector3D(1, 1, 1), aiQuaternion(0.976296, -0, -0, -0.2164396),
                            aiVector3D(9.411734, -2.738604, -2.866085));
            node->mTransformation.a1 = tmp.a1;
            node->mTransformation.a2 = tmp.a2;
            node->mTransformation.a3 = tmp.a3;
            node->mTransformation.a4 = tmp.a4;
            node->mTransformation.b1 = tmp.b1;
            node->mTransformation.b2 = tmp.b2;
            node->mTransformation.b3 = tmp.b3;
            node->mTransformation.b4 = tmp.b4;
            node->mTransformation.c1 = tmp.c1;
            node->mTransformation.c2 = tmp.c2;
            node->mTransformation.c3 = tmp.c3;
            node->mTransformation.c4 = tmp.c4;
            node->mTransformation.d1 = tmp.d1;
            node->mTransformation.d2 = tmp.d2;
            node->mTransformation.d3 = tmp.d3;
            node->mTransformation.d4 = tmp.d4;

            auto conn = Connection();
            conn.ConvertFromAiNode(node, ctx);

            pugi::xml_document doc;
            auto outNode = doc.append_child("connections");
            conn.ConvertToXml(outNode, ctx);


            auto offsetNode = outNode.child("connection").child("offset");
            auto posNode = offsetNode.child("position");
            auto quatNode = offsetNode.child("quaternion");

            // TODO make order of args right
            // TODO are we gonna bother flipping axes?
            BOOST_CHECK_EQUAL(posNode.attribute("x").as_float(), 9.411734f);
            BOOST_CHECK_EQUAL(posNode.attribute("y").as_float(), -2.738604f);
            BOOST_CHECK_EQUAL(posNode.attribute("z").as_float(), -2.866085f);
            // NB: ordering matching weird xml ordering
            BOOST_CHECK_EQUAL(quatNode.attribute("qx").as_float(), -0.0f);
            BOOST_CHECK_EQUAL(quatNode.attribute("qy").as_float(), -0.0f);
            BOOST_CHECK_EQUAL(quatNode.attribute("qz").as_float(), -0.2164396f);
            BOOST_CHECK_EQUAL(quatNode.attribute("qw").as_float(), 0.976296f);


            delete node;
        }

        BOOST_AUTO_TEST_CASE(from_xml_name) { // NOLINT(cert-err58-cpp)

            ConversionContext ctx(TestUtil::GetBasePath());
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, ctx);
            BOOST_TEST(conn.getName() == "Connection02");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_xml_no_parent) { // NOLINT(cert-err58-cpp)

            ConversionContext ctx(TestUtil::GetBasePath());
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection01']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, ctx, "ship_arg_s_fighter_01");
            BOOST_TEST(conn.getParentName() == "ship_arg_s_fighter_01");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(from_xml_has_parent) { // NOLINT(cert-err58-cpp)

            ConversionContext ctx(TestUtil::GetBasePath());
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, ctx);
            BOOST_TEST(conn.getParentName() == "anim_main");
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(xml_to_ainode_conn_attrs_tags) { // NOLINT(cert-err58-cpp)

            ConversionContext ctx(TestUtil::GetBasePath());
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->select_node("/components/component/connections/connection[@name='Connection02']").node();
            BOOST_TEST_REQUIRE(!node.empty());

            auto conn = Connection(node, ctx);
            auto result = conn.ConvertToAiNode(ctx);
            BOOST_TEST_REQUIRE(result->mNumChildren == 2);
//        BOOST_TEST(result->mChildren[0]->mName); TODO someday
            TestUtil::checkAiNodeName(result->mChildren[0],
                                      "Connection02|tags|part animation iklink nocollision forceoutline detail_xl  ");
            delete doc;
            delete result;
        }

        BOOST_AUTO_TEST_CASE(from_ainode_name) { // NOLINT(cert-err58-cpp)
            auto ainode = new aiNode("*Connection02*");

            ConversionContext ctx(TestUtil::GetBasePath());

            auto conn = Connection();
            conn.ConvertFromAiNode(ainode, ctx);
            BOOST_TEST(conn.getName() == "Connection02");
            delete ainode;
        }

        BOOST_AUTO_TEST_CASE(ainode_to_xml_conn_attrs_tags) { // NOLINT(cert-err58-cpp)

            ConversionContext ctx(TestUtil::GetBasePath());
            auto node = new aiNode("*Connection02*");
            auto children = new aiNode *[1];
            std::string tagStr = "Connection02|tags|part animation iklink nocollision forceoutline detail_xl  ";
            children[0] = new aiNode(tagStr);
            node->addChildren(1, children);
            pugi::xml_document doc;
            auto outNode = doc.append_child("connections");

            auto conn = Connection();
            conn.ConvertFromAiNode(node, ctx);
            conn.ConvertToXml(outNode, ctx);

            auto connNode = outNode.find_child_by_attribute("connection", "name", "Connection02");
            BOOST_TEST(std::string(connNode.attribute("tags").value()) ==
                       "part animation iklink nocollision forceoutline detail_xl  ");

            delete node;
            delete[] children;
        }

// TODO animations, etc
// TODO ship_arg_s_fighter_01 restrictions
    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
