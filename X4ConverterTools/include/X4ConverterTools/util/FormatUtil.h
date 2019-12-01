#ifndef X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
#define X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
#include <boost/format.hpp>
#include <cstring>
namespace util {
class FormatUtil {
 public:
  static std::string formatFloat(float f) {
    return str(boost::format("%.8e") % f);
  }
};
}
#endif //X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
