#include <boost/test/unit_test.hpp>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <X4ConverterTools/model/Light.h>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace model;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(LightUnitTests) // NOLINT(cert-err58-cpp)
auto AreaLight1 =
    R"(   <layer>
      <arealight name="XU AreaLight05" areax="0" areay="0.53" radius="0" r="124" g="255" b="255" lighteffect="1" range="0.5" spotattenuation="0.5" specularintensity="1" trigger="1" intensity="0.03">
      <lightanimations/><offset>
      <position x="-0.4923809" y="-1.759839" z="6.782082"/>
      <quaternion qx="-0" qy="-0" qz="-0.8660254" qw="0.5"/>
      </offset></arealight>
      </layer>)";
BOOST_AUTO_TEST_CASE(arealight_to_ailight) {
  // Context shouldn't matter for this test
//  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
//  pugi::xml_document doc;
//  doc.load_string(AreaLight1);
//  auto layerNode = doc.root().child("layer");
//  auto lightNode = layerNode.child("arealight");
//  Light l{lightNode, ctx, "test"};
//  auto out = l.ConvertToAiNode();// TODO
//  BOOST_REQUIRE(out.mType == aiLightSource_AREA);
//  BOOST_TEST(std::string(out.mName.C_Str()) == "test|light|XU AreaLight05");
//  BOOST_TEST(out.mSize.x == 0);
//  BOOST_TEST(out.mSize.y == 0.53f);
//  BOOST_TEST(out.mColorSpecular.r == (124.0f / 255.0f));
//  BOOST_TEST(out.mColorSpecular.g == 1.0f);
//  BOOST_TEST(out.mColorSpecular.b == 1.0f);
//  BOOST_TEST(out.mPosition.x == -0.4923809f);
//  BOOST_TEST(out.mPosition.y == -1.759839f);
//  BOOST_TEST(out.mPosition.z == 6.782082f);
  //BOOST_TEST(out->mUp == aiQuaternion(-0.0f,-0.0f,-0.8660254f, 0.5f)); // TODO
}
BOOST_AUTO_TEST_CASE(ailight_to_arealight) {
  // Context shouldn't matter for this test
//  auto ctx = TestUtil::GetTestContext(R"(assets\units\size_s\ship_arg_s_fighter_01)");
//  pugi::xml_document expectedDoc;
//  expectedDoc.load_string(AreaLight1);
//  aiLight light;
//  light.mName = "test|light|XU AreaLight05";
//  light.mType = aiLightSource_AREA;
//  light.mSize = aiVector2D(0, 0.53);
//  light.mColorSpecular = aiColor3D(124.0f / 255.0f, 1.0f, 1.0f);
//  light.mPosition = aiVector3D(-0.4923809f, -1.759839f, 6.782082f);
//
//  Light result (light, ctx);
//  pugi::xml_document actualDoc;
//  auto layerNode = actualDoc.append_child("layer");
//  result.ConvertToGameFormat(layerNode);
// TODO
//  TestUtil::CompareXMLFiles(&expectedDoc, &actualDoc);
  //BOOST_TEST(out->mUp == aiQuaternion(-0.0f,-0.0f,-0.8660254f, 0.5f)); // TODO
}
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
