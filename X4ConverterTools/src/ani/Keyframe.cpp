#include <X4ConverterTools/ani/Keyframe.h>
#include <iostream>
#include <cmath>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <X4ConverterTools/util/FormatUtil.h>
#include <sstream>
using namespace boost;
using namespace Assimp;
using namespace util;
namespace ani {
/**
 * Interpolation types
 * https://knowledge.autodesk.com/support/3ds-max/learn-explore/caas/CloudHelp/cloudhelp/2019/ENU/3DSMax-Animation/files/GUID-3A32AF50-18F8-4461-8B6C-AD36D5F668FF-htm.html
 * https://knowledge.autodesk.com/support/3ds-max/learn-explore/caas/CloudHelp/cloudhelp/2019/ENU/3DSMax-Animation/files/GUID-863F4C77-A476-41FF-908B-33C0095E21CB-htm.html
 * Ticks:
 * https://knowledge.autodesk.com/support/3ds-max/learn-explore/caas/CloudHelp/cloudhelp/2019/ENU/3DSMax-Animation/files/GUID-9E3D02C0-DAA9-4629-8558-6F6C070A0C70-htm.html
 * Length: https://help.autodesk.com/view/3DSMAX/2016/ENU/?guid=__files_GUID_76F76CB2_1026_447E_8DEE_23A57FB29C25_htm
 *
 * 3ds max - tangent/slope? + proportion relative to other keyframe
 *  blender - value (can calculate slope from this) + frame (x coordinate)
*/

Keyframe::Keyframe(StreamReaderLE &reader) {
  // 128 bytes
  reader >> ValueX >> ValueY >> ValueZ;
  reader >> InterpolationX >> InterpolationY >> InterpolationZ;
  reader >> Time;

  reader >> CPX1x >> CPX1y;
  reader >> CPX2x >> CPX2y;
  reader >> CPY1x >> CPY1y;
  reader >> CPY2x >> CPY2y;
  reader >> CPZ1x >> CPZ1y;
  reader >> CPZ2x >> CPZ2y;

  reader >> Tens;
  reader >> Cont;
  reader >> Bias;
  reader >> EaseIn;
  reader >> EaseOut;
  reader >> Deriv;
  reader >> DerivInX >> DerivInY >> DerivInZ;
  reader >> DerivOutX >> DerivOutY >> DerivOutZ;
  reader >> AngleKey;

  validate();

}
// Export Conversion

Keyframe::Keyframe(pugi::xml_node &xNode, pugi::xml_node &yNode, pugi::xml_node &zNode) {

  if (!xNode || !yNode || !zNode) {
    throw std::runtime_error("A keyframe must exist for all three axes or not at all");
  }
  if (strcmp(xNode.name(), "frame") != 0 || strcmp(yNode.name(), "frame") != 0 || strcmp(zNode.name(), "frame") != 0) {
    throw std::runtime_error("expected <frame> element");
  }
  auto frameTime = xNode.attribute("id").as_float();
  if (yNode.attribute("id").as_float() != frameTime || zNode.attribute("id").as_float() != frameTime) {
    throw std::runtime_error(
        "A key frame must exist for all three axes or not at all; the times for the different axes did not match");
  }
  Time = frameTime;
  ReadChannel(xNode, Axis::X);
  ReadChannel(yNode, Axis::Y);
  ReadChannel(zNode, Axis::Z);
}

void Keyframe::WriteToGameFiles(StreamWriterLE &writer) {
  auto startOffset = writer.GetCurrentPos();
  writer << ValueX << ValueY << ValueZ;
  writer << InterpolationX << InterpolationY << InterpolationZ;
  writer << Time;

  writer << CPX1x << CPX1y;
  writer << CPX2x << CPX2y;
  writer << CPY1x << CPY1y;
  writer << CPY2x << CPY2y;
  writer << CPZ1x << CPZ1y;
  writer << CPZ2x << CPZ2y;

  writer << Tens;
  writer << Cont;
  writer << Bias;
  writer << EaseIn;
  writer << EaseOut;
  writer << Deriv;
  writer << DerivInX << DerivInY << DerivInZ;
  writer << DerivOutX << DerivOutY << DerivOutZ;
  writer << AngleKey;
  auto finishOffset = writer.GetCurrentPos();
  if (finishOffset - startOffset != 128) {
    throw runtime_error("Keyframe should have been 128 bytes");
  }
}
std::string Keyframe::validate() {
  bool valid = true;
  std::stringstream ret;
  ret << "\tKeyFrame: \n";

  // TODO finish making use of stringstream
  ret << str(format("\t\tValue: (%1%, %2%, %3%)\n") % ValueX % ValueY % ValueZ);

  ret << str(format("\t\tInterpolation Types: (%1%, %2%, %3%)\n") %
      static_cast<std::underlying_type_t<InterpolationType>>(InterpolationX) %
      static_cast<std::underlying_type_t<InterpolationType>>(InterpolationY) %
      static_cast<std::underlying_type_t<InterpolationType>>(InterpolationZ));

  if (!checkInterpolationType(InterpolationX)) {
    valid = false;
    ret << "\t\tInterpolationX is not handled by the converter at present\n";
  }
  if (!checkInterpolationType(InterpolationY)) {
    valid = false;
    ret << "\t\tInterpolationY is not handled by the converter at present\n";
  }
  if (!checkInterpolationType(InterpolationZ)) {
    valid = false;
    ret << "\t\tInterpolationX is not handled by the converter at present\n";
  }
  ret << str(format("\t\tInterpolation Types - Readable: (%1%, %2%, %3%)\n") %
      GetInterpolationTypeStr(InterpolationX) % GetInterpolationTypeStr(InterpolationY) %
      GetInterpolationTypeStr(InterpolationZ));

  ret << str(format("\t\tTime: %1%\n") % Time);

  ret << str(format("\t\tCPX1: (%1%, %2%)\n") % CPX1x % CPX1y);
  ret << str(format("\t\tCPX2: (%1%, %2%)\n") % CPX2x % CPX2y);

  ret << str(format("\t\tCPY1: (%1%, %2%)\n") % CPY1x % CPY1y);
  ret << str(format("\t\tCPY2: (%1%, %2%)\n") % CPY2x % CPY2y);

  ret << str(format("\t\tCPZ1: (%1%, %2%)\n") % CPZ1x % CPZ1y);
  ret << str(format("\t\tCPZ2: (%1%, %2%)\n") % CPZ2x % CPZ2y);

  ret << str(format("\t\tTens: %1% Cont: %2% Bias: %3%\n") % Tens % Cont % Bias);
  ret << str(format("\t\tEaseIn: %1% EaseOut: %2%\n") % EaseIn % EaseOut);

  ret << str(format("\t\tDeriv: %1%\n") % Deriv);

  ret << str(format("\t\tDerivIn:  (%1%, %2%, %3%)\n") % DerivInX % DerivInY % DerivInZ);
  ret << str(format("\t\tDerivOut: (%1%, %2%, %3%)\n") % DerivOutX % DerivOutY % DerivOutZ);

  ret << str(format("\t\tAngleKey: %1%\n") % AngleKey);
  // Wrong
  // TODO what exactly was I checking here again?
//  float comp = std::numeric_limits<float>::min();
//  if (InterpolationX == 2) {
//    if (std::abs(CPX1x) > comp || std::abs(CPX1y) > comp | std::abs(CPX2x) > comp || std::abs(CPX2y) > comp) {
//      ret << "Interpolation Type for X was 2, but CP were not!\n";
//      valid = false;
//    }
//  }
//  if (InterpolationY == 2) {
//    if (std::abs(CPY1x) > comp || std::abs(CPY1y) > comp || std::abs(CPY2x) > comp || std::abs(CPY2y) > comp) {
//      ret << "Interpolation Type for Y was  2, but CP were not!\n";
//      valid = false;
//    }
//  }
//  if (InterpolationZ == 2) {
//    if (std::abs(CPZ1x) > comp || std::abs(CPZ1y) > comp || std::abs(CPZ2x) > comp || std::abs(CPZ2y) > comp) {
//      ret << "Interpolation Type for Z was 2, but CP were not!\n";
//      valid = false;
//    }
//  }

  if (Tens != 0) {
    ret << "Unsupported parameter: Tens";
    valid = false;
  }

  if (Cont != 0) {
    ret << "Unsupported parameter: Cont";
    valid = false;
  }

  if (Bias != 0) {
    ret << "Unsupported parameter: Bias";
    valid = false;
  }

  if (EaseIn != 0) {
    ret << "Unsupported parameter: EaseIn";
    valid = false;
  }

  if (EaseOut != 0) {
    ret << "Unsupported parameter: EaseOut";
    valid = false;
  }

  if (Deriv != 0 || DerivInX != 0 || DerivInY != 0 || DerivInZ != 0 || DerivOutX != 0 || DerivOutY != 0 ||
      DerivOutZ != 0) {
    ret << "Unsupported parameter(s): Deriv...";
    valid = false;
  }

  if (!valid) {
    throw std::runtime_error(ret.str());
  }
  return ret.str();
}

bool Keyframe::checkInterpolationType(InterpolationType type) {

  if (type == InterpolationType::INTERPOLATION_TCB) {

    std::cerr << "Warning, TCB Interpolation is not handled, but is valid for convenience." << std::endl;
    return true;
  }
  return (type == InterpolationType::INTERPOLATION_STEP || type == InterpolationType::INTERPOLATION_BEZIER
      || type == InterpolationType::INTERPOLATION_LINEAR);
}

void Keyframe::setValueByAxis(Axis axis, float value) {
  if (axis == Axis::X) {
    ValueX = value;
  } else if (axis == Axis::Y) {
    ValueY = value;
  } else if (axis == Axis::Z) {
    ValueY = value;
  } else {
    throw std::runtime_error("Invalid axis!");
  }
}

float Keyframe::getValueByAxis(Axis axis) {
  if (axis == Axis::X) {
    return ValueX;
  } else if (axis == Axis::Y) {
    return ValueY;
  } else if (axis == Axis::Z) {
    return ValueZ;
  } else {
    throw std::runtime_error("Invalid axis!");
  }
}

void Keyframe::setInterpByAxis(Axis axis, InterpolationType value) {
  if (axis == Axis::X) {
    InterpolationX = value;
  } else if (axis == Axis::Y) {
    InterpolationY = value;
  } else if (axis == Axis::Z) {
    InterpolationZ = value;
  } else {
    throw std::runtime_error("Invalid axis!");
  }
}

InterpolationType Keyframe::getInterpByAxis(Axis axis) {
  if (axis == Axis::X) {
    return InterpolationX;
  } else if (axis == Axis::Y) {
    return InterpolationY;
  } else if (axis == Axis::Z) {
    return InterpolationZ;
  } else {
    throw std::runtime_error("Invalid axis!");
  }
}

void Keyframe::ReadChannel(pugi::xml_node &node, Axis axis) {
  auto value = node.attribute("value").as_float();
  setValueByAxis(axis, value);
  ReadHandle(node, axis, false);
  ReadHandle(node, axis, true);
  auto interp = GetInterpolationType(node.attribute("interpolation").value());
  if (!checkInterpolationType(interp) || interp == InterpolationType::INTERPOLATION_TCB) {
    throw std::runtime_error("Cannot read keyframe");
  }
  setInterpByAxis(axis, interp);
}

void Keyframe::WriteChannel(pugi::xml_node &node, Axis axis) {
  InterpolationType interp = getInterpByAxis(axis);
  if (!checkInterpolationType(interp) || interp == InterpolationType::INTERPOLATION_TCB) {
    throw std::runtime_error("Cannot write keyframe");
  }
  auto tgtNode = node.append_child("frame");
  tgtNode.append_attribute("id").set_value(FormatUtil::formatFloat(Time).c_str());
  auto interpStr = GetInterpolationTypeStr(interp);
  auto value = getValueByAxis(axis);
  tgtNode.append_attribute("value").set_value(FormatUtil::formatFloat(value).c_str());
  WriteHandle(tgtNode, axis, false);
  WriteHandle(tgtNode, axis, true);
  tgtNode.append_attribute("interpolation").set_value(interpStr);

}

void Keyframe::WriteHandle(pugi::xml_node node, Axis axis, bool right) {
  pugi::xml_node tgtNode;
  if (right) {
    tgtNode = node.append_child("handle_left");
  } else {
    tgtNode = node.append_child("handle_right");
  }
  auto handle = getControlPoint(axis, right);
  tgtNode.append_attribute("X").set_value(FormatUtil::formatFloat(handle.first).c_str());
  tgtNode.append_attribute("Y").set_value(FormatUtil::formatFloat(handle.second).c_str());
}

void Keyframe::ReadHandle(pugi::xml_node node, Axis axis, bool right) {
  pugi::xml_node tgtNode;
  if (right) {
    tgtNode = node.child("handle_left");
  } else {
    tgtNode = node.child("handle_right");
  }
  auto handle = std::make_pair(tgtNode.attribute("X").as_float(), tgtNode.attribute("Y").as_float());
  setControlPoint(axis, handle, right);
}

std::pair<float, float> Keyframe::getControlPoint(Axis axis, bool right) {

  if (!right) {
    if (axis == Axis::X) {
      return std::make_pair(CPX1x, CPX1y);
    } else if (axis == Axis::Y) {
      return std::make_pair(CPY1x, CPY1y);
    } else if (axis == Axis::Z) {
      return std::make_pair(CPZ1x, CPZ1y);
    } else {
      throw std::runtime_error("Invalid axis!");
    }
  } else {
    if (axis == Axis::X) {
      return std::make_pair(CPX2x, CPX2y);
    } else if (axis == Axis::Y) {
      return std::make_pair(CPY2x, CPY2y);
    } else if (axis == Axis::Z) {
      return std::make_pair(CPZ2x, CPZ2y);
    } else {
      throw std::runtime_error("Invalid axis!");
    }
  }
}
void Keyframe::setControlPoint(Axis axis, std::pair<float, float> cp, bool right) {
  if (!right) {
    if (axis == Axis::X) {
      CPX1x = cp.first;
      CPX1y = cp.second;
    } else if (axis == Axis::Y) {
      CPY1x = cp.first;
      CPY1y = cp.second;
    } else if (axis == Axis::Z) {
      CPZ1x = cp.first;
      CPZ1y = cp.second;
    } else {
      throw std::runtime_error("Invalid axis!");
    }
  } else {
    if (axis == Axis::X) {
      CPX2x = cp.first;
      CPX2y = cp.second;
    } else if (axis == Axis::Y) {
      CPY2x = cp.first;
      CPY2y = cp.second;
    } else if (axis == Axis::Z) {
      CPZ2x = cp.first;
      CPZ2y = cp.second;
    } else {
      throw std::runtime_error("Invalid axis!");
    }
  }
}
float Keyframe::GetTime() {
  return Time;
}

}