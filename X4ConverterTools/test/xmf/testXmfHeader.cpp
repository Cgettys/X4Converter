#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/StreamReader.h>
#include <assimp/MemoryIOWrapper.h>
#include <X4ConverterTools/xmf/XmfHeader.h>

using namespace xmf;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_SUITE(XmfUnitTests) // NOLINT(cert-err58-cpp)
// TODO extract some permanent test files
BOOST_AUTO_TEST_CASE(xmf_header) {
  uint8_t bytes[] = {
      0x58, 0x55, 0x4d, 0x46, // XUMF
      0x03, // Version = 3
      0x00, // Endianness
      0x40, // 64 byte header
      0x00, // Reserved
      0x42, // 66 data buffers
      0xbc, // Each description is 188 bytes (0xbc)
      0x05, // 5 materials
      0x88, // Material struct is 136 bytes
      0x00, // Culling Clockwise = false
      0x00, // Righthand = false
      0x6a, 0x00, 0x00, 0x00, // NumVertices
      0x2c, 0x01, 0x00, 0x00, // NumIndices
      0x04, 0x00, 0x00, 0x00, // PrimitiveType=TriangleList
      0x00, 0x00, 0x00, 0x00, // MeshOptimization = none?

      // TODO testme
      0x00, 0x00, 0x00, 0x00, // Bounding Box Center X
      0x00, 0x00, 0x00, 0x00, // Bounding Box Center Y
      0x00, 0x00, 0x00, 0x00, // Bounding Box Center Z
      0x00, 0x00, 0x00, 0x00, // Bounding Box Size X
      0x00, 0x00, 0x00, 0x00, // Bounding Box Size Y
      0x00, 0x00, 0x00, 0x00, // Bounding Box Size Z

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // padding
      // Test to make sure it doesn't go over by adding some extra garbage in the array
      // (prevents segfaults if it goes over)
      0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
      0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
      0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
      0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF
  };
  auto pStream = new MemoryIOStream(bytes, sizeof(bytes), false);
  BOOST_TEST_CHECKPOINT("Setup complete");

  StreamReaderLE pStreamReader(pStream);
  XmfHeader header(pStreamReader);
  BOOST_TEST_CHECKPOINT("Read complete");
  BOOST_TEST(pStreamReader.GetCurrentPos() == 0x40);
  BOOST_TEST_MESSAGE(header.validate());
  BOOST_TEST_CHECKPOINT("Validation complete");
  BOOST_TEST(header.NumDataBuffers == 66);
  BOOST_TEST(header.NumMaterials == 5);
  BOOST_TEST(header.NumVertices == 106);
  BOOST_TEST(header.NumIndices == 300);
  BOOST_TEST(header.PrimitiveType == 4);
  // pStream is "helpfully" cleaned up by pStreamReader
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()