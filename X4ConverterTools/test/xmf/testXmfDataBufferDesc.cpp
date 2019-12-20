#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//TODO uncompressed tests

#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/StreamReader.h>
#include <assimp/MemoryIOWrapper.h>
#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>

using namespace Assimp;
using namespace xmf;
BOOST_AUTO_TEST_SUITE(UnitTests)
BOOST_AUTO_TEST_SUITE(XmfUnitTests)
// TODO extract some permanent test files
BOOST_AUTO_TEST_CASE(xmf_data_buffer_desc) {
  // assets/units/size_s/ship_gen_s_fighter_01_data/fx_licence-collision.xmf v2.20 or so
  uint8_t bytes[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xaa, 0x04, 0x00, 0x00, 0x6a, 0x00,
                     0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xb0,
                     0x5a, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0xb0, 0x5a, 0x0f, 0x40, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80,
                     0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x20, 0xf6, 0xe0, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x98, 0xde, 0x97, 0x5a, 0xff, 0x7f, 0x00, 0x00, 0x48, 0x21, 0x8e, 0x3f, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x4b, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0xe8, 0xc0, 0x04, 0x0e, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0xaa, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00,
                     0x00, 0x00, 0x6e, 0x01, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x40, 0x5a, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xe6, 0xfd,
                     0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x4b, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
  IOStream *pStream = new MemoryIOStream(bytes, sizeof(bytes), false);
  BOOST_TEST_CHECKPOINT("Setup complete");

  StreamReaderLE pStreamReader(pStream, false);
  XmfDataBufferDesc desc(pStreamReader);
  BOOST_TEST_CHECKPOINT("Read complete");
  BOOST_TEST(pStreamReader.GetCurrentPos() == 188);
  // TODO real validation?
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
#pragma clang diagnostic pop