#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/StreamReader.h>
#include <assimp/MemoryIOWrapper.h>
#include <X4ConverterTools/xmf/XmfHeader.h>

using namespace xmf;
using namespace Assimp;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)

    BOOST_AUTO_TEST_SUITE(XmfUnitTests) // NOLINT(cert-err58-cpp)
// TODO extract some permanent test files
        BOOST_AUTO_TEST_CASE(xmf_header) {
            // assets/units/size_s/ship_gen_s_fighter_01_data/fx_licence-collision.xmf v2.20 or so
            uint8_t bytes[] = {0x58, 0x55, 0x4d, 0x46, 0x03, 0x00, 0x40, 0x00, 0x02, 0xbc, 0x00, 0x88, 0x00, 0x00, 0x6a,
                               0x00, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE,
                               0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD,
                               0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,};// extra bytes to make sure it didn't go over
            IOStream *pStream = new MemoryIOStream(bytes, sizeof(bytes), false);
            BOOST_TEST_CHECKPOINT("Setup complete");

            StreamReaderLE pStreamReader(pStream, false);
            XmfHeader header(pStreamReader);
            BOOST_TEST_CHECKPOINT("Read complete");
            BOOST_TEST(pStreamReader.GetCurrentPos() == 0x40);
            BOOST_TEST_MESSAGE(header.validate());
            // pStream is "helpfully" cleaned up by pStreamReader
        }

    BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()