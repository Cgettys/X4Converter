#include <boost/test/unit_test.hpp>


#include <X4ConverterTools/model/Part.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/model/Component.h>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(ComponentUnitTests) // NOLINT(cert-err58-cpp)

        BOOST_AUTO_TEST_CASE(from_xml_basic) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->root();
            BOOST_TEST_REQUIRE(!node.empty());

            auto component = Component(node);
            BOOST_TEST(component.getName() == "ship_arg_s_fighter_01");
            BOOST_TEST(component.getNumberOfConnections() == 74);
            delete doc;
        }

        BOOST_AUTO_TEST_CASE(xml_to_ainode_basic) { // NOLINT(cert-err58-cpp)
            auto doc = TestUtil::GetXmlDocument("/assets/units/size_s/ship_arg_s_fighter_01.xml");
            auto node = doc->root();
            BOOST_TEST_REQUIRE(!node.empty());
            auto component = Component(node);

            auto result = component.ConvertToAiNode();

            BOOST_TEST_REQUIRE(result->mNumChildren > 0);
            TestUtil::checkAiNodeName(result->mChildren[0], "ship_arg_s_fighter_01");

            delete doc;
            delete result;
        }


// TODO layers?
    BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
