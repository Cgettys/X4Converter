#define BOOST_TEST_MODULE test
#include <vector>
#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>
#include <X4ConverterTools/Types.h>
#include <X4ConverterTools/Xmf/XuMeshFile.h>
using namespace Assimp;
BOOST_AUTO_TEST_SUITE( test_suite1 )

BOOST_AUTO_TEST_CASE( test_xmf )
{
	// TODO do we want to mock reading & writing to memory - would be faster & good form
	// See https://github.com/assimp/assimp/blob/master/include/assimp/MemoryIOWrapper.h
	const std::string testFile = "/home/cg/Desktop/X4/unpacked/extensions/break/assets/units/size_s/ship_gen_s_fighter_02_data/detail_m_int-lod0.xmf";
	const std::string resultsFile = "/home/cg/Desktop/X4/unpacked/extensions/break/assets/units/size_s/ship_gen_s_fighter_02_data/detail_m_int-lod0.out.xmf";
	IOSystem* io = new DefaultIOSystem();
	std::shared_ptr<XuMeshFile> meshFile = XuMeshFile::ReadFromFile(testFile,io);
	meshFile->WriteToFile(resultsFile,io);
	IOStream* expectedStream= io->Open(testFile,"rb");
	IOStream* actualStream= io->Open(resultsFile,"rb");
	size_t expectedLen=expectedStream->FileSize();
	size_t actualLen=actualStream->FileSize();
	std::vector<byte> expected(expectedLen);
	std::vector<byte> actual(actualLen);
	//	 Directly read the data into the vector & make sure we got it one go)
	BOOST_TEST(expectedLen == expectedStream->Read(&expected[0],sizeof(byte),expectedLen));
	BOOST_TEST(actualLen == actualStream->Read(&actual[0],sizeof(byte),actualLen));


//	actualStream->Seek(0,aiOrigin_SET);
//	XmfHeader actualHeader = XuMeshFile::ReadHeader(actualStream);
	io->Close(expectedStream);
	io->Close(actualStream);
	BOOST_TEST(expected == actual);


}

BOOST_AUTO_TEST_SUITE_END()


