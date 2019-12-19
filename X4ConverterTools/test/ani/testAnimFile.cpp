#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

#include <X4ConverterTools/ani/AnimFile.h>
#include <boost/filesystem.hpp>
#include "../testUtil.h"

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace ani;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(AnimUnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_CASE(ani_read_basic) { // NOLINT(cert-err58-cpp)
  // TODO mock reading & writing to memory - would be faster & good form
  // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
  auto aniFile =
      test::TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  AnimFile file(sourceStream);
  std::cout << file.validate();
}

BOOST_AUTO_TEST_CASE(ani_out) { // NOLINT(cert-err58-cpp)
  // TODO fixme
  auto aniFile =
      test::TestUtil::GetBasePath() / "assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
  auto xmlFile =
      test::TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.xml";
  auto aniOutFile =
      test::TestUtil::GetBasePath() / "assets/units/size_s/ship_gen_s_fighter_01.out.anixml";
//auto aniFile = "test::TestUtil::GetBasePath()/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        auto aniFile =current_path().string()+"/assets/fx/lensflares/LENSFLARES_DATA.ANI";
  std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
  IOStream *sourceStream = io->Open(aniFile.string(), "rb");
  BOOST_TEST_REQUIRE(sourceStream != nullptr);
  AnimFile file(sourceStream);
  std::cout << file.validate();

  // TODO fixme

  pugi::xml_document doc;
  file.WriteIntermediateRepr(xmlFile.string(), doc.root());
  doc.save_file(aniOutFile.c_str());//TODO get rid of this once we validate

  // TODO validate
}

//        BOOST_AUTO_TEST_CASE(ani_both) { // NOLINT(cert-err58-cpp)
//            // TODO fixme
//            const std::string aniFile =
//                    test::TestUtil::GetBasePath() + "/assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
//            const std::string xmlFile =
//                    test::TestUtil::GetBasePath() + "/assets/units/size_s/ship_gen_s_fighter_01.xml";
//
//            std::unique_ptr<IOSystem> io = std::make_unique<DefaultIOSystem>();
//            IOStream *sourceStream = io->Open(aniFile, "rb");
//            BOOST_TEST_REQUIRE(sourceStream != nullptr);
//            AnimFile file(sourceStream);
//            std::string expected = file.validate();
//            std::cout << "Expected:\n" << expected;
//
//            // TODO fixme
//
//            pugi::xml_document doc;
//            file.WriteIntermediateRepr(xmlFile, doc.root());
//            AnimFile reverse = AnimFile(doc.root());
//            std::string actual = reverse.validate();
//            std::cout << "Actual:\n" << actual;
//            BOOST_TEST(expected == actual);
//            // TODO validate better
//        }


BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(ValidateData)

BOOST_AUTO_TEST_CASE(ani_struct_correctness) { // NOLINT(cert-err58-cpp)
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
      try {
        AnimFile file(sourceStream);
        std::cout << filePath.c_str() << std::endl;
        file.validate();
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