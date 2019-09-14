#include <boost/test/unit_test.hpp>
#include <assimp/types.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultIOSystem.h>
#include <X4ConverterTools/Conversion.h>
#include <X4ConverterTools/xmf/XmfHeader.h>
#include <X4ConverterTools/xmf/XmfFile.h>
#include <boost/filesystem.hpp>
#include "../testUtil.h"

using namespace boost;
namespace fs = boost::filesystem;
using namespace Assimp;
using namespace xmf;
using namespace test;
BOOST_AUTO_TEST_SUITE(UnitTests) // NOLINT(cert-err58-cpp)
BOOST_AUTO_TEST_SUITE(XmfUnitTests) // NOLINT(cert-err58-cpp)

BOOST_AUTO_TEST_CASE(xmf_material) {
  // TODO writeme
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()