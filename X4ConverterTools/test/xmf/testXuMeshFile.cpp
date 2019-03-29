
#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include <assimp/DefaultIOSystem.h>
#include <assimp/MemoryIOWrapper.h>
#include <X4ConverterTools/API.h>
#include <X4ConverterTools/types.h>
#include <X4ConverterTools/xmf/XmfHeader.h>
#include <X4ConverterTools/xmf/XuMeshFile.h>


using namespace Assimp;
using namespace xmf;
BOOST_AUTO_TEST_SUITE(test_suite1)

    class testHelper {
    public:
        //TODO seperate test suite for validating current files
// TODO refactor out of test file?
        static void checkXmfHeaderEquality(XuMeshFile &lFile, XuMeshFile &rFile) {
            XmfHeader lhs = lFile.GetHeader();
            XmfHeader rhs = rFile.GetHeader();

            //    byte Culling_CW;                    // byte       12; false == CCW, other is CW TODO validate
            //    byte RightHand;                     // byte       13; 0 == lefthand D3D convention, other is righthand openGL format todo validate
            //    dword NumVertices;                  // bytes 14 - 17; TODO use for validation
            //    dword NumIndices;                   // bytes 18 - 21; TODO use for validation
            //
            //    dword PrimitiveType;                // bytes 22 - 25;
            //    dword MeshOptimization;	            // bytes 26 - 29; MeshOptimizationType - enum MeshOptimizationType applied
            //    float BoundingBoxCenter[3];		    // bytes 30 - 41; virtual center of the mesh TODO calculate
            //    float BoundingBoxSize[3];		    // bytes 42 - 53; max absolute extents from the center (aligned coords) TODO calculate
            //    byte pad[10];                       // bytes 54 - 63
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

        }

        static void checkXuMeshFileEquality(XuMeshFile &lhs, XuMeshFile &rhs) {
            checkXmfHeaderEquality(lhs, rhs);
            // TODO write nice equality functions for this
//            BOOST_TEST( lhs.GetBuffers() == rhs.GetBuffers());
//            BOOST_TEST( lhs.GetMaterials() == rhs.GetMaterials());
        }
    };


    BOOST_AUTO_TEST_CASE(test_xmf_material) {
        // TODO writeme
    }

    BOOST_AUTO_TEST_CASE(test_xmf) {

        // TODO mock reading & writing to memory - would be faster & good form
        // See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h

        const std::string basePath =
                "/home/cg/Desktop/X4/unpacked/extensions/break/assets/units/size_s/ship_gen_s_fighter_02_data/fx_licence-collision";
        const std::string testFile = basePath + ".xmf";
        const std::string resultsFile = basePath + ".out.xmf";
        IOSystem *io = new DefaultIOSystem();
        IOStream *sourceStream = io->Open(testFile, "rb");
        IOStream *outStream = io->Open(resultsFile, "wb");
        std::shared_ptr<XuMeshFile> meshFile = XuMeshFile::ReadFromIOStream(sourceStream);
        meshFile->WriteToIOStream(outStream);
        BOOST_TEST_CHECKPOINT("Read/Write complete");

        // Reset stream to start
        IOStream *resultStream = io->Open(resultsFile, "rb");
        sourceStream = io->Open(testFile, "rb");

        size_t sourceLen = sourceStream->FileSize();
        size_t resultLen = resultStream->FileSize();
        std::vector<byte> sourceBytes(sourceLen);
        std::vector<byte> resultByte(resultLen);
        // First layer of checks - Read the result in again - is it equal to what it just wrote out
        std::shared_ptr<XuMeshFile> reloadedFile = XuMeshFile::ReadFromIOStream(resultStream);

        testHelper::checkXuMeshFileEquality(*meshFile, *reloadedFile);
        //	 Directly read the data into the vector & make sure we got it one go)
        // TODO eventually would be better if we got exact same contents
//        BOOST_TEST(
//                sourceLen == sourceStream->Read(&expected[0], sizeof(byte), expectedLen));
//        BOOST_TEST(
//                actualLen == resultStream->Read(&actual[0], sizeof(byte), actualLen));

        io->Close(sourceStream);
        delete io;
    }

BOOST_AUTO_TEST_SUITE_END()