#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>
#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/xmf/XmfHeader.h>
#include <X4ConverterTools/xmf/XmfFile.h>
#include <boost/filesystem.hpp>
#include "../testUtil.h"

using namespace boost;
namespace fs = boost::filesystem;
using namespace Assimp;
using namespace xmf;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(XmfUnitTests) // NOLINT(cert-err58-cpp)
//TODO seperate test suite for validating current files

BOOST_AUTO_TEST_CASE(xmf_geom) {

  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h

  auto ctx = TestUtil::GetTestContext("extensions/break/assets/units/size_s/ship_gen_s_fighter_02_data/");
  const std::string tgt = "fx_licence-lod0";
  const std::string testFile = tgt + ".xmf";
  const std::string resultsFile = tgt + ".out.xmf";
  auto sourceStream = ctx->GetSourceFile(testFile, "rb");
  auto outStream = ctx->GetSourceFile(resultsFile, "wb");
  XmfFile::Ptr meshFile = XmfFile::ReadFromIOStream(sourceStream, ctx);
  meshFile->WriteToIOStream(outStream);
  BOOST_TEST_CHECKPOINT("Read/Write complete");

  // TODO do this bit in memory
  // Reset stream to start
  auto resultStream = ctx->GetSourceFile(resultsFile, "rb");
  sourceStream = ctx->GetSourceFile(testFile, "rb");

  size_t sourceLen = sourceStream->FileSize();
  size_t resultLen = resultStream->FileSize();
  std::vector<uint8_t> sourceBytes(sourceLen);
  std::vector<uint8_t> resultByte(resultLen);
  // First layer of checks - Read the result in again - is it equal to what it just wrote out
  XmfFile::Ptr reloadedFile = XmfFile::ReadFromIOStream(resultStream, ctx);

  TestUtil::checkXuMeshFileEquality(*meshFile, *reloadedFile);
  //	 Directly read the data into the vector & make sure we got it one go)
  // TODO eventually would be better if we got exact same contents
//        BOOST_TEST(
//                sourceLen == sourceStream->Read(&expected[0], sizeof(byte), expectedLen));
//        BOOST_TEST(
//                actualLen == resultStream->Read(&actual[0], sizeof(byte), actualLen));

}
BOOST_AUTO_TEST_CASE(xmf_collision) {

  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h

  auto ctx = TestUtil::GetTestContext("extensions/break/assets/units/size_s/ship_gen_s_fighter_02_data");
  const std::string tgt = "fx_licence-collision";
  const std::string testFile = tgt + ".xmf";
  const std::string resultsFile = tgt + ".out.xmf";
  auto sourceStream = ctx->GetSourceFile(testFile, "rb");
  auto outStream = ctx->GetSourceFile(resultsFile, "wb");
  XmfFile::Ptr meshFile = XmfFile::ReadFromIOStream(sourceStream, ctx);
  meshFile->WriteToIOStream(outStream);
  BOOST_TEST_CHECKPOINT("Read/Write complete");

  // Reset stream to start
  // TODO memory stream
  auto resultStream = ctx->GetSourceFile(resultsFile, "rb");
  sourceStream = ctx->GetSourceFile(testFile, "rb");

  size_t sourceLen = sourceStream->FileSize();
  size_t resultLen = resultStream->FileSize();
  std::vector<uint8_t> sourceBytes(sourceLen);
  std::vector<uint8_t> resultByte(resultLen);
  // First layer of checks - Read the result in again - is it equal to what it just wrote out
  XmfFile::Ptr reloadedFile = XmfFile::ReadFromIOStream(resultStream, ctx);

  TestUtil::checkXuMeshFileEquality(*meshFile, *reloadedFile);
  //	 Directly read the data into the vector & make sure we got it one go)
  // TODO eventually would be better if we got exact same contents
//        BOOST_TEST(
//                sourceLen == sourceStream->Read(&expected[0], sizeof(byte), expectedLen));
//        BOOST_TEST(
//                actualLen == resultStream->Read(&actual[0], sizeof(byte), actualLen));

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ValidateData)

BOOST_AUTO_TEST_CASE(xmf_struct_correctness) { // NOLINT(cert-err58-cpp)
  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
  const fs::path basePath = fs::path(std::getenv("X4_UNPACKED_ROOT"));
  fs::recursive_directory_iterator iter(basePath);
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  for (const auto &x : iter) {
    const fs::path &filePath = x.path();
    if (filePath.has_extension() && iequals(filePath.extension().generic_string(), ".xmf") &&
        !iends_with(filePath.filename().generic_string(), ".out.xmf")) {
//                std::cout << filePath << std::endl;

      auto sourceStream = io->Open(filePath.generic_string(), "rb");
      try {
        auto file = XmfFile::ReadFromIOStream(sourceStream, nullptr);//TODO not the null demon
        std::cout << filePath.c_str() << std::endl;
      } catch (std::runtime_error &e) {
        std::string error = str(format("Filepath: %1% Exception:\n %2%\n") % filePath.c_str() % e.what());
        // Change to BOOST_CHECK_MESSAGE if you want all the files violating the structure
        BOOST_REQUIRE_MESSAGE(false, error);
      }
    }
  }
  // To make a confusing warning go away
  BOOST_REQUIRE_MESSAGE(true, "No files should have errors");
}

BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)