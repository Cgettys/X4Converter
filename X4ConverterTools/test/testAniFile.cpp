
#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

//#include <X4ConverterTools/API.h>
//#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Ani/AniFile.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <stdexcept>

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(test_suite1)


    BOOST_AUTO_TEST_CASE(test_ani_read_basic) {
        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
        const std::string aniFile =
                "/home/cg/Desktop/X4/unpacked/assets/units/size_s/DOOR_ARG_S_SCOUT_01_DATA.ANI";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(aniFile, "rb");
        BOOST_TEST_REQUIRE(sourceStream != nullptr);
        AniFile *file = AniFile::ReadFromIOStream(sourceStream);
        std::cout << file->validate();
        io->Close(sourceStream);
        delete io;
        delete file;
    }



    // TODO test suite for this?
    BOOST_AUTO_TEST_CASE(test_ani_struct_correctness) {
        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
        const fs::path basePath = fs::path("/home/cg/Desktop/X4/unpacked/");
        fs::recursive_directory_iterator iter(basePath);
        IOSystem *io = new DefaultIOSystem();
        for (auto x : iter) {
            const fs::path filePath = x.path();
            if (filePath.has_extension() && filePath.extension() == ".ANI") {
//                std::cout << filePath << std::endl;

                IOStream *sourceStream = io->Open(filePath.c_str(), "rb");
                try {
                    AniFile *file = AniFile::ReadFromIOStream(sourceStream);
                    file->validate();
                    if (file->getHeader()->usedUnknown1!=1){
                        BOOST_REQUIRE_MESSAGE(false, filePath.c_str());
                    }
                    delete file;
                } catch (std::runtime_error e) {
                    std::string error = str(format("Filepath: %1% Exception:\n %2%\n")% filePath.c_str() % e.what());
                    // Change to BOOST_CHECK_MESSAGE if you want all the files violating the structure
                    BOOST_REQUIRE_MESSAGE(false, error);
                }
                io->Close(sourceStream);

            }
        }
        // To make a confusing warning go away
        BOOST_REQUIRE_MESSAGE(true,"No files should have errors");
        delete io;
    }


BOOST_AUTO_TEST_SUITE_END()