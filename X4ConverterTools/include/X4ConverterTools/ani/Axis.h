#pragma once
namespace ani {

// Used to make internal code a bit less messy
enum class Axis { X, Y, Z };

inline const char *GetAxisString(Axis axis) {
  switch (axis) {
    case Axis::X:
      return "X";
    case Axis::Y:
      return "Y";
    case Axis::Z:
      return "Z";
    default:
      throw std::runtime_error("Invalid Axis value");
  }
}
}