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

BOOST_AUTO_TEST_CASE(arealight_to_ainode){
  auto ctx = TestUtil::GetTestContext("");
  std::stringstream ss;
  ss << R"(<arealight name="XU AreaLight05" areax="0" areay="0.53" radius="0" r="124" g="255" b="255" lighteffect="1" range="0.5" spotattenuation="0.5" specularintensity="1" trigger="1" intensity="0.03">)";
  ss << R"(<lightanimations/><offset>)";
  ss << R"(<position x="-0.4923809" y="-1.759839" z="6.782082"/>)";
  ss << R"(<quaternion qx="-0" qy="-0" qz="-0.8660254" qw="0.5"/>)";
  ss << R"(</offset></arealight>)";
  std::string xml = ss.str();
  pugi::xml_document doc;
  doc.load_string(xml.c_str());
  auto node = doc.root().first_child();
  Light l {node,ctx, "test"};
  auto* out = ctx->GetLight("test-light-XU AreaLight05");
  BOOST_REQUIRE(out->mType == aiLightSource_AREA);
  BOOST_ASSERT(out->mSize == aiVector2D(0, 0.53));
  BOOST_ASSERT(out->mColorSpecular == aiColor3D(124.0f/255.0f, 1.0f, 1.0f));
  BOOST_ASSERT(out->mPosition == aiVector3D(-0.4923809f, -1.759839f, 6.782082f));
  //BOOST_ASSERT(out->mUp == aiQuaternion(-0.0f,-0.0f,-0.8660254f, 0.5f)); // TODO
}
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
