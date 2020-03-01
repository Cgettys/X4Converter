#include <cmath>
#include <utility>

#include "testUtil.h"

#include <set>
#include <algorithm>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <regex>
#include <cmath>
#include <X4ConverterTools/xmf/XmfHeader.h>
#include <boost/filesystem/path.hpp>
#include <X4ConverterTools/util/XmlUtil.h>

namespace test {

void TestUtil::checkAiNodeName(aiNode *node, std::string name) {

  BOOST_TEST_REQUIRE(node != nullptr);
  const char *actualName = node->mName.C_Str();
  BOOST_TEST(name == std::string(actualName));
}

std::unique_ptr<pugi::xml_document> TestUtil::GetXmlDocument(const std::string &path) {
  boost::filesystem::path fullPath = path;
  if (fullPath.is_relative()) {
    fullPath = GetBasePath() / path;
  } else {
    BOOST_FAIL("Provide relative path in place of this: " + path);
  }
  auto pathString = fullPath.make_preferred().c_str();
  auto doc = std::make_unique<pugi::xml_document>();
  pugi::xml_parse_result result = doc->load_file(pathString);
  BOOST_TEST_REQUIRE(result.status == pugi::status_ok);
  return doc;
}

boost::filesystem::path TestUtil::GetBasePath() {

  char *path = std::getenv("X4_UNPACKED_ROOT");
  BOOST_TEST_REQUIRE(path != nullptr);
  return {path};
}

ConversionContext::Ptr TestUtil::GetTestContext(const boost::filesystem::path &tgtPath, bool convert) {
  // TODO better checks
  if (tgtPath.is_absolute()) {
    BOOST_FAIL("Provide relative paths to test files");
  }
  auto fsUtil = std::make_shared<util::FileSystemUtil>(GetBasePath().string(), true);
  auto metadataPath = tgtPath.generic_path().replace_extension(".out.metadata").make_preferred().string();
  auto ctx = std::make_shared<ConversionContext>(fsUtil, metadataPath, convert);
  ctx->fsUtil->SetSourcePathSuffix(tgtPath.generic_path().replace_extension("").string() + "_data");
  return ctx;
}

void TestUtil::CompareXMLFiles(pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc) {

  //https://pugixml.org/docs/manual.html#access.walker
  // See also: https://stackoverflow.com/a/29599648
  struct my_walker : pugi::xml_tree_walker {
    std::set<std::string> paths;

    bool for_each(pugi::xml_node &node) override {
      std::vector<std::string> pathParts;
      auto parent = node;
      if (node.attributes_begin() == node.attributes().end()) {
        return true;
      }
      do {
        std::string part = parent.name();
        if (parent.attribute("name")) {
          part.append("[@name='");
          part.append(parent.attribute("name").as_string());
          part.append("']");
        } else if (parent.attribute("id")) {
          part.append("[@id='");
          part.append(parent.attribute("id").as_string());
          part.append("']");
        } else if (parent.attribute("index")) {
          part.append("[@index='");
          part.append(parent.attribute("index").as_string());
          part.append("']");
        }
        pathParts.emplace(pathParts.begin(), part);
        parent = parent.parent();
      } while (parent);
//			std::cout << path << std::endl;
      // Duplicate element == BAD
//			BOOST_TEST(!paths.count(path));
      std::string path = "/";
      for (auto &x : pathParts) {
        path.append(x);
        path.append("/");
      }
      path.pop_back();
      paths.insert(path);
      return true; // continue traversal
    }
  };
  my_walker expectedWalk;
  expectedDoc->traverse(expectedWalk);
  my_walker actualWalk;
  actualDoc->traverse(actualWalk);
  std::set<std::string> expectedMinusActual;
  std::set_difference(expectedWalk.paths.begin(),
                      expectedWalk.paths.end(),
                      actualWalk.paths.begin(),
                      actualWalk.paths.end(),
                      std::inserter(expectedMinusActual, expectedMinusActual.begin()));

  std::set<std::string> actualMinusExpected;
  std::set_difference(actualWalk.paths.begin(),
                      actualWalk.paths.end(),
                      expectedWalk.paths.begin(),
                      expectedWalk.paths.end(),
                      std::inserter(actualMinusExpected, actualMinusExpected.begin()));

  std::set<std::string> intersection;
  std::set_intersection(actualWalk.paths.begin(),
                        actualWalk.paths.end(),
                        expectedWalk.paths.begin(),
                        expectedWalk.paths.end(),
                        std::inserter(intersection, intersection.begin()));
  BOOST_TEST(expectedMinusActual.size() == 0);
  for (auto &&x : expectedMinusActual) {
    printf("Output was missing path: %s\n", x.c_str());
  }
  BOOST_TEST(actualMinusExpected.size() == 0);
  for (auto &&x : actualMinusExpected) {
    printf("Output has extra path: %s\n", x.c_str());
  }

  for (auto &&x : intersection) {
    checkPath(x, expectedDoc, actualDoc);
  }
}
void TestUtil::checkQuaternion(const std::string path, aiQuaternion &expectedQuat, aiQuaternion &actualQuat) {
  if (!expectedQuat.Equal(actualQuat)) {
    if (expectedQuat.Equal({-actualQuat.w, -actualQuat.x, -actualQuat.y, -actualQuat.z})) {
      BOOST_TEST_WARN(false, "Negated quaternion at path: " + path + ". This may cause problems for interpolation!");
    } else {
      BOOST_CHECK_MESSAGE(false,
                          "Expected quaternion: (" +
                              std::to_string(expectedQuat.w) + "," +
                              std::to_string(expectedQuat.x) + "," +
                              std::to_string(expectedQuat.y) + "," +
                              std::to_string(expectedQuat.z) +
                              ") Actual Quaternion: (" +
                              std::to_string(actualQuat.w) + "," +
                              std::to_string(actualQuat.x) + "," +
                              std::to_string(actualQuat.y) + "," +
                              std::to_string(actualQuat.z) + ")");
    }
  }
}
void TestUtil::checkPath(const std::string &path, pugi::xml_document *expectedDoc, pugi::xml_document *actualDoc) {
  auto expectedNode = expectedDoc->select_node(path.c_str()).node();
  auto actualNode = actualDoc->select_node(path.c_str()).node();
  // Slightly modified to allow integers too for ease of use: from https://stackoverflow.com/a/5578251
  auto re_float = std::regex(
      R"(^\s*[+-]?([0-9]+\.?[0-9]*([Ee][+-]?[0-9]+)?|\.?[0-9]+([Ee][+-]?[0-9]+)?|[0-9]+[Ee][+-]?[0-9]+)$)");
  // TODO maybe reconsider quaternion check? technically not equivalent for some forms of interpolation
  if (std::string(expectedNode.name()) == "quaternion") {
    auto expectedQuat = util::XmlUtil::ReadAttrQuat(expectedNode);
    auto actualQuat = util::XmlUtil::ReadAttrQuat(actualNode);
    checkQuaternion(path, expectedQuat, actualQuat);
  } else {
    for (auto &expectedAttr : expectedNode.attributes()) {
      std::string attrName = expectedAttr.name();
      auto actualAttr = actualNode.attribute(attrName.c_str());
      BOOST_TEST(actualAttr, "Missing attribute at path: " + path + " attr name: " + attrName);
      if (actualAttr) {
        std::string expectedValueStr = expectedAttr.as_string();
        std::string actualValueStr = actualAttr.as_string();
        if (!
            std::regex_match(expectedValueStr, re_float
            )) {
          BOOST_TEST_WARN(expectedValueStr == actualValueStr,
                          "String attribute " + attrName + " at path " + path + " should be equal\n" +
                              "Expected: " + expectedValueStr + " Actual: " + actualValueStr);
        } else {
          float expectedValueFloat = expectedAttr.as_float();
          float actualValueFloat = actualAttr.as_float();
          const float small = 2e-6;
          BOOST_TEST_INFO("Path: " + path
                              + "\n\tattr: " + attrName
                              + "\n\tExpected: " + std::to_string(expectedValueFloat)
                              + "\n\tActual: " + std::to_string(actualValueFloat));
          if ((std::fabs(expectedValueFloat) > small) || (std::fabs(actualValueFloat) > small)) {
            BOOST_CHECK_CLOSE(expectedValueFloat, actualValueFloat, 1.0);
          } else {
            BOOST_CHECK_SMALL(expectedValueFloat, small);
            BOOST_CHECK_SMALL(actualValueFloat, small);
          }

        }
      }

    }

    for (auto &actualAttr: actualNode.attributes()) {
      BOOST_TEST(expectedNode.attribute(actualAttr.name()),
                 "Extra attribute at path: " + path + " attr name: " + actualAttr.name());
    }
  }
}
using namespace xmf;

void TestUtil::checkXuMeshFileEquality(XmfFile &lhs, XmfFile &rhs) {
  checkXmfHeaderEquality(lhs, rhs);
  // TODO write nice equality functions for this
//            BOOST_TEST( lhs.GetBuffers() == rhs.GetBuffers());
//            BOOST_TEST( lhs.GetMaterials() == rhs.GetMaterials());
}

void TestUtil::checkXmfHeaderEquality(XmfFile &lFile, XmfFile &rFile) {
  XmfHeader lhs = lFile.GetHeader();
  XmfHeader rhs = rFile.GetHeader();

  //    byte Culling_CW;                    // byte       12; false == CCW, other is CW TODO validate
  //    byte RightHand;                     // byte       13; 0 == lefthand D3D convention, other is righthand openGL format todo validate
  //    uint32_t NumVertices;                  // bytes 14 - 17; TODO use for validation
  //    uint32_t NumIndices;                   // bytes 18 - 21; TODO use for validation
  //
  //    uint32_t PrimitiveType;                // bytes 22 - 25;
  //    uint32_t MeshOptimization;	            // bytes 26 - 29; MeshOptimizationType - enum MeshOptimizationType applied
  //    float BoundingBoxCenter[3];		    // bytes 30 - 41; virtual center of the mesh TODO calculate
  //    float BoundingBoxSize[3];		    // bytes 42 - 53; max absolute extents from the center (aligned coords) TODO calculate
  //    byte pad[10];                       // bytes 54 - 63
  // Check that the two are equal
  BOOST_TEST(lhs.Magic == rhs.Magic);
  BOOST_TEST(lhs.Version == rhs.Version);
  BOOST_TEST(lhs.IsBigEndian == rhs.IsBigEndian);
  BOOST_TEST(lhs.SizeOfHeader == rhs.SizeOfHeader);
  BOOST_TEST(lhs.reserved0 == rhs.reserved0);

  BOOST_TEST(lhs.NumDataBuffers == rhs.NumDataBuffers);
  BOOST_TEST(lhs.DataBufferDescSize == rhs.DataBufferDescSize);
  BOOST_TEST(lhs.NumMaterials == rhs.NumMaterials);
  BOOST_TEST(lhs.MaterialSize == rhs.MaterialSize);

  BOOST_TEST(lhs.Culling_CW == rhs.Culling_CW);
  BOOST_TEST(lhs.RightHand == rhs.RightHand);
  BOOST_WARN(lhs.NumVertices == rhs.NumVertices);  // TODO turn these into tests
  BOOST_WARN(lhs.NumIndices == rhs.NumIndices);
  BOOST_TEST(lhs.PrimitiveType == rhs.PrimitiveType);

  BOOST_WARN(lhs.MeshOptimization == rhs.MeshOptimization); // TODO turn these into tests
  BOOST_WARN(lhs.BoundingBoxCenter == rhs.BoundingBoxCenter);
  BOOST_WARN(lhs.BoundingBoxSize == rhs.BoundingBoxSize);

  BOOST_TEST(lhs.pad == rhs.pad);

  // The equality function should agree:
  BOOST_TEST((lhs == rhs));
}

}