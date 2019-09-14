//
// Created by cg on 4/29/19.
//

#pragma once

#include <string>

#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace util {

class FormatUtil {
 public:
  static std::string formatFloat(float f) {
    return str(boost::format("%.8e") % f);
  }
};
}