
#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

//#include <X4ConverterTools/API.h>
//#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Ani/AniFile.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <assimp/scene.h>
#include "pugixml.hpp"
namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(test_suite1) // NOLINT(cert-err58-cpp)


    BOOST_AUTO_TEST_CASE(test_ani_read_basic) { // NOLINT(cert-err58-cpp)
        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
        const std::string aniFile =
                "/home/cg/Desktop/X4/unpacked/assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
//const std::string aniFile = "/home/cg/Desktop/X4/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        const std::string aniFile ="/home/cg/Desktop/X4/unpacked/assets/fx/lensflares/LENSFLARES_DATA.ANI";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(aniFile, "rb");
        BOOST_TEST_REQUIRE(sourceStream != nullptr);
        AniFile file = AniFile(sourceStream);
        std::cout << file.validate();
        delete io;
    }

    BOOST_AUTO_TEST_CASE(test_ani_playground) { // NOLINT(cert-err58-cpp)
        Assimp::Importer* importer = new Assimp::Importer();
        const aiScene *pScene = importer->ReadFile("/home/cg/Desktop/X4/untitled.dae", 0);
        BOOST_TEST(pScene);
//        BOOST_TEST(pScene->HasAnimations());
 }
    BOOST_AUTO_TEST_CASE(test_ani_out) { // NOLINT(cert-err58-cpp)
        const std::string aniFile =
                "/home/cg/Desktop/X4/unpacked/assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
//const std::string aniFile = "/home/cg/Desktop/X4/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        const std::string aniFile ="/home/cg/Desktop/X4/unpacked/assets/fx/lensflares/LENSFLARES_DATA.ANI";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(aniFile, "rb");
        BOOST_TEST_REQUIRE(sourceStream != nullptr);
        AniFile file = AniFile(sourceStream);
        std::cout << file.validate();

        pugi::xml_document doc;
        file.WriteAnims(doc.root());
        doc.save(std::cout);

        delete io;
    }
    // TODO test suite for this?
    BOOST_AUTO_TEST_CASE(test_ani_struct_correctness) { // NOLINT(cert-err58-cpp)
        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
        const fs::path basePath = fs::path("/home/cg/Desktop/X4/unpacked/");
        fs::recursive_directory_iterator iter(basePath);
        IOSystem *io = new DefaultIOSystem();
        for (const auto &x : iter) {
            const fs::path &filePath = x.path();
            if (filePath.has_extension() && iequals(filePath.extension().generic_string(), ".ANI")) {
//                std::cout << filePath << std::endl;

                auto sourceStream = io->Open(filePath.c_str(), "rb");
                try {
                    AniFile file(sourceStream);
                    std::cout << filePath.c_str() << std::endl;
                    file.validate();
                } catch (std::runtime_error& e) {
                    std::string error = str(format("Filepath: %1% Exception:\n %2%\n")% filePath.c_str() % e.what());
                    // Change to BOOST_CHECK_MESSAGE if you want all the files violating the structure
                    BOOST_REQUIRE_MESSAGE(false, error);
                }
            }
        }
        // To make a confusing warning go away
        BOOST_REQUIRE_MESSAGE(true,"No files should have errors");
        delete io;
    }


BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)