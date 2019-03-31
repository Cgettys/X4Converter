#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

#include <X4ConverterTools/ani/AnimFile.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <assimp/scene.h>
#include "pugixml.hpp"
#include <cstdio>
#include <cmath>

namespace fs = boost::filesystem;
using namespace boost;
using namespace Assimp;
using namespace ani;
BOOST_AUTO_TEST_SUITE(test_suite1) // NOLINT(cert-err58-cpp)


    BOOST_AUTO_TEST_CASE(test_ani_read_basic) { // NOLINT(cert-err58-cpp)
        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
        const std::string aniFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
//const std::string aniFile = "/home/cg/Desktop/X4/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        const std::string aniFile ="/home/cg/Desktop/X4/unpacked/assets/fx/lensflares/LENSFLARES_DATA.ANI";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(aniFile, "rb");
        BOOST_TEST_REQUIRE(sourceStream != nullptr);
        AnimFile file(sourceStream);
        std::cout << file.validate();
        delete io;
    }

    BOOST_AUTO_TEST_CASE(test_ani_playground) { // NOLINT(cert-err58-cpp)
        Assimp::Importer *importer = new Assimp::Importer();
        const aiScene *pScene = importer->ReadFile("/home/cg/Desktop/X4/untitled.dae", 0);
        BOOST_TEST(pScene);
//        BOOST_TEST(pScene->HasAnimations());
    }

    bool matrixPred(pugi::xml_node node) {
        return strncmp(node.name(), "matrix", 6) == 0;
    }

    BOOST_AUTO_TEST_CASE(test_ani_out) { // NOLINT(cert-err58-cpp)
    // TODO fixme
        const std::string aniFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/SHIP_GEN_S_FIGHTER_01_DATA.ANI";
        const std::string xmlFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.xml";
        const std::string aniOutFile = "/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.2.dae";
//const std::string aniFile = "/home/cg/Desktop/X4/test_files/struct_bt_ut_omicron_superyard_data.ani";
//        const std::string aniFile ="/home/cg/Desktop/X4/unpacked/assets/fx/lensflares/LENSFLARES_DATA.ANI";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(aniFile, "rb");
        BOOST_TEST_REQUIRE(sourceStream != nullptr);
        AnimFile file(sourceStream);
        std::cout << file.validate();

//        pugi::xml_document doc;
//        doc.load_file("/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.out.dae");
//        pugi::xml_node tgt = doc.root().child("COLLADA").child("library_animations");
//        if (tgt.empty()) {
//            std::cout << "appending child" << std::endl;
//            tgt = doc.root().child("COLLADA").append_child("library_animations");
//        }
        //Hackity hack hack hack...
        // Attempt to transform matrices exported by assimp into posrotloc required by blender animations
//        struct simple_walker : pugi::xml_tree_walker {
//            virtual bool for_each(pugi::xml_node &node) {
//                if (strncmp(node.name(),"matrix",6)==0) {
//                    float m[16];
//                    sscanf(node.text().get(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
//                           m + 0, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7,
//                           m + 8, m + 9, m + 10, m + 11, m + 12, m + 13, m + 14, m + 15);
//
//                    pugi::xml_node parent = node.parent();
//                    auto translateNode = parent.insert_child_before("translate",node);
//                    translateNode.append_attribute("sid").set_value("location");
//                    translateNode.append_child(pugi::node_pcdata).set_value(
//                            str(format("%1% %2% %3%") % m[3] % m[7] % m[11]).c_str());
//                    m[3] = 0;
//                    m[7] = 0;
//                    m[11] = 0;
//
//                    float scale[3];
//                    scale[0] = sqrt(m[0] * m[0] + m[4] * m[4] + m[8] * m[8]);
//                    scale[1] = sqrt(m[1] * m[1] + m[5] * m[5] + m[9] * m[9]);
//                    scale[2] = sqrt(m[2] * m[2] + m[6] * m[6] + m[10] * m[10]);
//
//                    for (int x = 0; x < 3; x ++){
//                        for (int y = 0; y < 3; y++){
//                            m[x+y*4] /= scale[x]; // TODO checkme
//                        }
//                    }
//
//                    auto rotZNode = parent.insert_child_before("rotate",node);
//                    rotZNode.append_attribute("sid").set_value("rotationX");
//                    rotZNode.append_child(pugi::node_pcdata).set_value(
//                            str(format("%1% %2% %3% %4%") % m[0] % m[1] % m[2] %m[3]).c_str());
//
//                    auto rotYNode = parent.insert_child_before("rotate",node);
//                    rotYNode.append_attribute("sid").set_value("rotationY");
//                    rotYNode.append_child(pugi::node_pcdata).set_value(
//                            str(format("%1% %2% %3% %4%") % m[4] % m[5] % m[6] %m[7]).c_str());
//
//
//                    auto rotXNode = parent.insert_child_before("rotate",node);
//                    rotXNode.append_attribute("sid").set_value("rotationZ");
//                    rotXNode.append_child(pugi::node_pcdata).set_value(
//                            str(format("%1% %2% %3% %4%") % m[8] % m[9] % m[10] %m[11]).c_str());
//
//
//                    auto scaleNode = parent.insert_child_before("scale",node);
//                    scaleNode.append_attribute("sid").set_value("scale");
//                    scaleNode.append_child(pugi::node_pcdata).set_value(
//                            str(format("%1% %2% %3%") % scale[0] % scale[1] % scale[2]).c_str());
//
//                }
//                return true; // continue traversal
//            }
//
//        };

        pugi::xml_document doc;
        doc.load_file("/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.dae");
        pugi::xml_node rt = doc.root().child("asset").append_child("library_animations");
        file.WriteAnims(xmlFile, rt);
        doc.save_file(aniOutFile.c_str());
//
//        simple_walker walker;
//        doc.traverse(walker);
//        pugi::xml_node n = doc.find_node(matrixPred);
//        while (n){
//            n.parent().remove_child(n);
//            n = doc.find_node(matrixPred);
//        }

//        doc.save_file("/home/cg/Desktop/X4/unpacked/assets/units/size_s/ship_gen_s_fighter_01.out.2.dae");

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
        delete io;
    }


BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)