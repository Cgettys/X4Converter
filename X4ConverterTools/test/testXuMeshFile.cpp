
#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>

#include <X4ConverterTools/API.h>
#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Xmf/XuMeshFile.h>

using namespace Assimp;
BOOST_AUTO_TEST_SUITE(test_suite1)

    class testHelper {
    public:
// TODO refactor out of test file?
        static void checkXmfHeaderEquality(XuMeshFile &lFile, XuMeshFile &rFile) {
            XmfHeader lhs = lFile.GetHeader();
            XmfHeader rhs = rFile.GetHeader();
            //        byte Magic[4];
            //        byte Version;
            //        bool BigEndian;
            //        byte DataBufferDescOffset;
            //        byte _pad0 = 0;
            //        byte NumDataBuffers;
            //        byte DataBufferDescSize;
            //        byte NumMaterials;
            //        byte MaterialSize;
            //        byte _pad1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            //        dword PrimitiveType;
            BOOST_TEST(lhs.Magic == rhs.Magic);
            BOOST_TEST(lhs.Version == rhs.Version);
            BOOST_TEST(lhs.BigEndian == rhs.BigEndian);
            BOOST_TEST(lhs.DataBufferDescOffset == rhs.DataBufferDescOffset);
            BOOST_WARN_MESSAGE(lhs._pad0 == rhs._pad0, "Padding byte _pad0 may differ");
            BOOST_TEST(lhs.NumDataBuffers == rhs.NumDataBuffers);
            BOOST_TEST(lhs.DataBufferDescSize == rhs.DataBufferDescSize);
            BOOST_TEST(lhs.NumMaterials == rhs.NumMaterials);
            BOOST_TEST(lhs.MaterialSize == rhs.MaterialSize);
            BOOST_WARN_MESSAGE(lhs._pad1 == rhs._pad1, "Padding byte[] _pad1  may differ");
            BOOST_TEST(lhs.PrimitiveType == rhs.PrimitiveType);

        }

        static void checkXuMeshFileEquality(XuMeshFile &lhs, XuMeshFile &rhs) {
            checkXmfHeaderEquality(lhs, rhs);
            // TODO write nice equality functions for this
//            BOOST_TEST( lhs.GetBuffers() == rhs.GetBuffers());
//            BOOST_TEST( lhs.GetMaterials() == rhs.GetMaterials());
        }
    };

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
        resultStream->Seek(0, aiOrigin_SET);
        testHelper::checkXuMeshFileEquality(*meshFile, *reloadedFile);
        //	 Directly read the data into the vector & make sure we got it one go)
        // TODO eventually would be better if we got exact same contents
//        BOOST_TEST(
//                sourceLen == sourceStream->Read(&expected[0], sizeof(byte), expectedLen));
//        BOOST_TEST(
//                actualLen == resultStream->Read(&actual[0], sizeof(byte), actualLen));

//	actualStream->Seek(0,aiOrigin_SET);
        io->Close(sourceStream);
        io->Close(resultStream);
        delete io;
//        BOOST_TEST(expected == actual);
    }


BOOST_AUTO_TEST_SUITE_END()