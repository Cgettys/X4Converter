#ifndef X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
#define X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
#include <boost/format.hpp>
#include <cstring>
#include <cmath>
namespace util {
class FormatUtil {
 public:
  static std::string formatFloat(float f) {
    auto result = str(boost::format("%.8e") % f);
    if (f < 0.0 && result[0] != '-') {
      throw std::runtime_error("Negative floats must have negative sign when serialized");
    }
    return result;
  }
  static float parseFloat(pugi::xml_attribute attr) {
    auto val = attr.value();
    auto result = std::stof(val);
    if (val[0] == '-' and !std::signbit(result)) {
      throw std::runtime_error("Negative floats must have negative sign when deserialized");
    }
    return result;
  }
};
}
#endif //X4CONVERTER_X4CONVERTERTOOLS_INCLUDE_X4CONVERTERTOOLS_UTIL_FORMATUTIL_H_
