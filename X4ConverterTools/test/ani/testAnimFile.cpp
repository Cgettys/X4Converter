#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

#include <X4ConverterTools/ani/AnimFile.h>
#include <boost/filesystem.hpp>
#include "../testUtil.h"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace ani;
using namespace test;

BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(AnimUnitTests)

BOOST_AUTO_TEST_CASE(ani_read_basic) {
  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader (sourceStream);
  AnimFile file(pStreamReader);
  std::cout << file.validate();
}

BOOST_AUTO_TEST_CASE(ani_read_basic_2) {
  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/DOOR_ARG_S_FIGHTER_01_DATA.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader (sourceStream);
  AnimFile file(pStreamReader);
  std::cout << file.validate();
}

BOOST_AUTO_TEST_CASE(ani_header) { // NOLINT(cert-err58-cpp)
  // TODO fixme
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";

  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader (sourceStream);
  // Copy the first 16 bytes for later comparison
  AnimFile file(pStreamReader);
  auto fwdHeader = file.GetHeader();
  BOOST_REQUIRE_EQUAL(fwdHeader.getNumAnims(), file.descs.size());
  // std::cout << file.validate();

  pugi::xml_document doc;
  auto rootNode = doc.root();
  file.WriteIntermediateRepr(xmlFile.string(), rootNode);
  // doc.save(std::cout);

  AnimFile reverse(rootNode);
  auto revHeader = reverse.GetHeader();
  BOOST_CHECK_EQUAL(fwdHeader.getNumAnims(), revHeader.getNumAnims());
  BOOST_CHECK_EQUAL(fwdHeader.getKeyOffsetBytes(),revHeader.getKeyOffsetBytes());
  BOOST_CHECK_EQUAL(fwdHeader.getVersion(), revHeader.getVersion());

  // Not really the header's fault, but let's check
  BOOST_CHECK_EQUAL(fwdHeader.getNumAnims(), reverse.descs.size());
  for (int i = 0; i < file.descs.size() && i < reverse.descs.size(); i++) {
    auto fwdDesc = file.descs[i];
    auto revDesc = reverse.descs[i];
    BOOST_REQUIRE_EQUAL(fwdDesc.SafeName+"/"+fwdDesc.SafeSubName, revDesc.SafeName+"/"+revDesc.SafeSubName);
  }
}

BOOST_AUTO_TEST_CASE(ani_inmemory_roundtrip) { // NOLINT(cert-err58-cpp)
  // TODO fixme
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";
  auto aniOutFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.out.anixml";
  auto aniBinOutFile = TestUtil::GetBasePath()  / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.out.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  // -prefersinglefiles -logfile debug.txt -debug all
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader (sourceStream);

  // Copy ani file for later comparison
  auto fileSize = pStreamReader.GetRemainingSize();
  std::vector<char> buffer;
  buffer.resize(fileSize);
  pStreamReader.CopyAndAdvance(buffer.data(), fileSize);
  pStreamReader.SetCurrentPos(0);

  AnimFile file(pStreamReader);
  std::cout << file.validate();



  IOStream *sinkStream = io->Open(aniBinOutFile.string(), "wb");
  BOOST_TEST_REQUIRE(sinkStream != nullptr);
  Assimp::StreamWriterLE pStreamWriter (sinkStream);

  pugi::xml_document doc;
  auto node = doc.root();
  file.WriteGameFiles(pStreamWriter, node);

  BOOST_TEST_REQUIRE(fileSize ==  pStreamWriter.GetCurrentPos());
  pStreamWriter.Flush();
  sinkStream->Flush();

  IOStream *checkStream = io->Open(aniBinOutFile.string(), "rb");
  BOOST_TEST_REQUIRE(checkStream != nullptr);
  Assimp::StreamReaderLE pCheckReader (checkStream);
  auto resultSize = pCheckReader.GetRemainingSize();
  std::vector<char> resultBuffer;
  resultBuffer.resize(resultSize);
  pCheckReader.CopyAndAdvance(resultBuffer.data(), resultSize);

  BOOST_TEST_REQUIRE(fileSize == resultSize);
  BOOST_TEST(buffer == resultBuffer, boost::test_tools::per_element());

  // TODO validate
}

BOOST_AUTO_TEST_CASE(ani_double) { // NOLINT(cert-err58-cpp)
  // File path for input, output
  auto aniFile = TestUtil::GetBasePath() / "X2PEGASUS_DATA.ANI";
  auto aniBinOutFile = TestUtil::GetBasePath() / "X2PEGASUS_DATA_OUT.ANI";
  // Setup input
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader(sourceStream);

  // Read in ANI file
  AnimFile file(pStreamReader);
  // Print details of file
  std::cout << file.validate();

  // Find the animations we care about; create copies with new names
  std::vector<AnimDesc> interestingAnims;
  for (const auto &desc: file.descs) {
    if (desc.SafeName.compare("anim_ladder") == 0 || desc.SafeName.compare("anim_ladder_board") == 0) {
      AnimDesc descCopy(desc);
      descCopy.SafeName = descCopy.SafeName + "2";
      interestingAnims.push_back(descCopy);
    }
  }
  // We expected 8 of them based on reading through the output of file.validate
  BOOST_TEST_REQUIRE(interestingAnims.size() == 8);

  // Add them to the file
  for (auto &desc : interestingAnims) {
    file.descs.push_back(desc);
  }

  IOStream *sinkStream = io->Open(aniBinOutFile.string(), "wb");
  BOOST_TEST_REQUIRE(sinkStream != nullptr);
  Assimp::StreamWriterLE pStreamWriter(sinkStream);

  pugi::xml_document doc;
  auto node = doc.root();
  file.WriteGameFiles(pStreamWriter, node);
  std::cout << file.validate();


  // TODO validate
}


BOOST_AUTO_TEST_CASE(ani_out) { // NOLINT(cert-err58-cpp)
  // TODO fixme
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";
  auto aniOutFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.out.anixml";
//auto aniFile = "test::TestUtil::GetBasePath()/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        auto aniFile =current_path().string()+"/assets/fx/lensflares/LENSFLARES_DATA.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader (sourceStream);
  AnimFile file(pStreamReader);
  std::cout << file.validate();

  // TODO fixme

  pugi::xml_document doc;
  auto node = doc.root();
  file.WriteIntermediateRepr(xmlFile.string(), node);
  doc.save_file(aniOutFile.c_str());//TODO get rid of this once we validate

  // TODO validate
}

BOOST_AUTO_TEST_CASE(ani_envy) { // NOLINT(cert-err58-cpp)
  // File path for input, output
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_m/ENVY_DATA.ANI";
  auto aniBinOutFile = TestUtil::GetBasePath() / "X2PEGASUS_DATA_OUT.ANI";
  // Setup input
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader(sourceStream);

  // Read in ANI file
  AnimFile file(pStreamReader);
  // Print details of file
  std::cout << file.validate();

  // TODO validate
}

BOOST_AUTO_TEST_CASE(ani_xml_roundtrip_header) { // NOLINT(cert-err58-cpp)

  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";

  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader(sourceStream);
  AnimFile file(pStreamReader);
  std::string expected = file.validate();
  std::cout << "Expected:\n" << expected;


  pugi::xml_document doc;
  auto rootNode = doc.root();
  file.WriteIntermediateRepr(xmlFile.string(), rootNode);
  doc.save(std::cout);

  AnimFile reverse(rootNode);
  BOOST_TEST(file.GetHeader().validate() == reverse.GetHeader().validate());
}

BOOST_AUTO_TEST_CASE(ani_xml_roundtrip) { // NOLINT(cert-err58-cpp)
  // TODO fixme
  auto aniFile = TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile = TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";

  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  Assimp::StreamReaderLE pStreamReader(sourceStream);
  AnimFile file(pStreamReader);
  std::string expected = file.validate();
  //std::cout << "Expected:\n" << expected;

  // TODO fixme

  pugi::xml_document doc;
  auto rootNode = doc.root();
  file.WriteIntermediateRepr(xmlFile.string(), rootNode);
  //doc.save(std::cout);

  AnimFile reverse(rootNode);
  std::string actual = reverse.validate();
  //std::cout << "Actual:\n" << actual;
  BOOST_TEST(expected == actual);
  // TODO validate better
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ValidateData)

BOOST_AUTO_TEST_CASE(ani_struct_correctness) {
  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
  const fs::path basePath = fs::path(std::getenv("X4_UNPACKED_ROOT"));
  fs::recursive_directory_iterator iter(basePath);
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  for (const auto &x : iter) {
    const fs::path &filePath = x.path();
    if (filePath.has_extension() && iequals(filePath.extension().generic_string(), ".ANI")) {
//                std::cout << filePath << std::endl;

      auto sourceStream = io->Open(filePath.generic_string(), "rb");
      Assimp::StreamReaderLE pStreamReader (sourceStream);
      try {
        AnimFile file(pStreamReader);
        std::cout << filePath.c_str() << std::endl;
        file.validate();
      } catch (std::runtime_error &e) {
        std::string error = str(format("Filepath: %1% Exception:\n %2%\n") % filePath.c_str() % e.what());
        // Change to BOOST_CHECK_MESSAGE if you want all the files violating the structure
        BOOST_CHECK_MESSAGE(false, error);
      }
    }
  }
  // To make a confusing warning go away
  BOOST_REQUIRE_MESSAGE(true, "No files should have errors");
}

BOOST_AUTO_TEST_SUITE_END()
#pragma clang diagnostic pop