#pragma once

#include <assimp/StreamReader.h>
#include <assimp/StreamWriter.h>
#include <utility>
#include <pugixml.hpp>
#include "Axis.h"
#include <unordered_map>

namespace ani {
enum class InterpolationType : uint32_t {
  INTERPOLATION_UNKNOWN,                /**< Unknown interpolation. */
  INTERPOLATION_STEP,                    /**< No interpolation. */
  INTERPOLATION_LINEAR,                /**< Linear interpolation.*/
  INTERPOLATION_QUADRATIC,
  INTERPOLATION_CUBIC,
  INTERPOLATION_BEZIER,                /**< Bezier interpolation with interpolated time. */
  INTERPOLATION_BEZIER_LINEARTIME,    /**< Bezier interpolation with linear time. */
  INTERPOLATION_TCB                    /**< TCB interpolation (using tension, continuity and bias generated hermite tangents). */
};
inline constexpr const char *GetInterpolationTypeStr(InterpolationType interp) {
  switch (interp) {
    case InterpolationType::INTERPOLATION_UNKNOWN:
      return "UNKNOWN";
    case InterpolationType::INTERPOLATION_STEP:
      return "STEP";
    case InterpolationType::INTERPOLATION_LINEAR:
      return "LINEAR";
    case InterpolationType::INTERPOLATION_QUADRATIC:
      return "QUADRATIC";
    case InterpolationType::INTERPOLATION_CUBIC:
      return "CUBIC";
    case InterpolationType::INTERPOLATION_BEZIER:
      return "BEZIER";
    case InterpolationType::INTERPOLATION_BEZIER_LINEARTIME:
      return "BEZIER_LINEARTIME";
    case InterpolationType::INTERPOLATION_TCB:
      return "TCB";
    default:
      throw std::runtime_error("Type name not in enumeration");
  }
}
const std::unordered_map<std::string, InterpolationType> interpolationMap = {
    {"UNKNOWN", InterpolationType::INTERPOLATION_UNKNOWN},
    {"STEP", InterpolationType::INTERPOLATION_STEP},
    {"LINEAR", InterpolationType::INTERPOLATION_LINEAR},
    {"QUADRATIC", InterpolationType::INTERPOLATION_QUADRATIC},
    {"CUBIC", InterpolationType::INTERPOLATION_CUBIC},
    {"BEZIER", InterpolationType::INTERPOLATION_BEZIER},
    {"BEZIER_LINEARTIME", InterpolationType::INTERPOLATION_BEZIER_LINEARTIME},
    {"TCB", InterpolationType::INTERPOLATION_TCB}
};
inline const InterpolationType GetInterpolationType(std::string interpStr) {
  auto result = interpolationMap.find(interpStr);
  if (result == interpolationMap.end()) {
    throw std::runtime_error("Type name not in enumeration");
  }
  return result->second;
}
class Keyframe {
 public:
  Keyframe() = default;

  explicit Keyframe(pugi::xml_node &xNode, pugi::xml_node &yNode, pugi::xml_node &zNode);

  explicit Keyframe(Assimp::StreamReaderLE &reader);

  void WriteToGameFiles(Assimp::StreamWriterLE &writer);

  std::string validate();// Debug method - throws exception if invalid, else returns human readable string

  static bool checkInterpolationType(InterpolationType type);

  void ReadChannel(pugi::xml_node &node, Axis axis);
  void WriteChannel(pugi::xml_node &node, Axis axis);

  float GetTime();

 protected:
  void setValueByAxis(Axis axis, float value);
  float getValueByAxis(Axis axis);

  void setInterpByAxis(Axis axis, InterpolationType value);
  InterpolationType getInterpByAxis(Axis axis);
  void ReadHandle(pugi::xml_node node, Axis axis, bool right);
  void WriteHandle(pugi::xml_node node, Axis axis, bool right);
  void setControlPoint(Axis axis, std::pair<float, float> cp, bool right);
  std::pair<float, float> getControlPoint(Axis axis, bool right);

  // Note that these add up to exactly 128 bytes
  float ValueX, ValueY, ValueZ;                          /**< The key's actual value (position, rotation, etc.). 12*/
  InterpolationType InterpolationX;                      /**< The type of interpolation for the x part of the key. 20*/
  InterpolationType InterpolationY;                      /**< The type of interpolation for the y part of the key. 24*/
  InterpolationType InterpolationZ;                      /**< The type of interpolation for the z part of the key. 28*/
  float Time;

  /**< 32 Time in s of the key frame - based on the start of the complete animation. - This value is also used as a unique identifier for the key meaning that two keys with the same time are considered the same! - We use a float rather than an XTIME to safe memory, because floating point precision is good enough for key times. */

  float CPX1x, CPX1y;                                    /**< First control point for the x value. 8*/
  float CPX2x, CPX2y;                                    /**< Second control point for the x value. 16*/
  float CPY1x, CPY1y;                                /**< First control point for the y value. 24*/
  float CPY2x, CPY2y;                                /**< Second control point for the y value. 32*/
  float CPZ1x, CPZ1y;                                /**< First control point for the z value. 40*/
  float CPZ2x, CPZ2y;                                /**< Second control point for the z value. 48*/

  float Tens = 0;                                            /**< Tension. 4*/
  float Cont = 0;                                            /**< Continuity. 8*/
  float Bias = 0;                                            /**< Bias. 12*/
  float EaseIn = 0;                                        /**< Ease In. 16*/
  float EaseOut = 0;                                        /**< Ease Out. 20*/
  int Deriv =
      0;                                            /**< 24 Indicates whether derivatives have been calculated already. Is mutable to allow it being altered in the CalculateDerivatives() method. */
  float DerivInX = 0, DerivInY = 0, DerivInZ =
      0;                    /**< 12 Derivative In value. Is mutable to allow it being altered in the CalculateDerivatives() method. */
  float DerivOutX = 0, DerivOutY = 0, DerivOutZ =
      0;                /**< 24 Derivative Out value.  Is mutable to allow it being altered in the CalculateDerivatives() method.*/
  uint32_t AngleKey;                                        /** 28		// this will be set to non null if there is a key */

};
}