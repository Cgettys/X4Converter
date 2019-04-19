#include <X4ConverterTools/ani/Keyframe.h>

using namespace boost;
using namespace Assimp;

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

    Keyframe::Keyframe(pugi::xml_node node) {

    }

    std::string Keyframe::validate() {
        bool valid = true;
        std::string ret = "\tKeyFrame: \n";

        ret.append(str(format("\t\tValue: (%1%, %2%, %3%)\n") % ValueX % ValueY % ValueZ));

        ret.append(str(format("\t\tInterpolation Types: (%1%, %2%, %3%)\n") % InterpolationX % InterpolationY %
                       InterpolationZ));


        if (!checkInterpolationType(InterpolationX)) {
            valid = false;
            ret.append("\t\tInterpolationX is not handled by the converter at present\n");
        }
        if (!checkInterpolationType(InterpolationY)) {
            valid = false;
            ret.append("\t\tInterpolationY is not handled by the converter at present\n");
        }
        if (!checkInterpolationType(InterpolationZ)) {
            valid = false;
            ret.append("\t\tInterpolationX is not handled by the converter at present\n");
        }
        ret.append(str(format("\t\tInterpolation Types - Readable: (%1%, %2%, %3%)\n") %
                       getInterpolationTypeName(InterpolationX) % getInterpolationTypeName(InterpolationY) %
                       getInterpolationTypeName(InterpolationZ)));


        ret.append(str(format("\t\tTime: %1%\n") % Time));

        ret.append(str(format("\t\tCPX1: (%1%, %2%)\n") % CPX1x % CPX1y));
        ret.append(str(format("\t\tCPX2: (%1%, %2%)\n") % CPX2x % CPX2y));

        ret.append(str(format("\t\tCPY1: (%1%, %2%)\n") % CPY1x % CPY1y));
        ret.append(str(format("\t\tCPY2: (%1%, %2%)\n") % CPY2x % CPY2y));

        ret.append(str(format("\t\tCPZ1: (%1%, %2%)\n") % CPZ1x % CPZ1y));
        ret.append(str(format("\t\tCPZ2: (%1%, %2%)\n") % CPZ2x % CPZ2y));

        ret.append(str(format("\t\tTens: %1% Cont: %2% Bias: %3%\n") % Tens % Cont % Bias));
        ret.append(str(format("\t\tEaseIn: %1% EaseOut: %2%\n") % EaseIn % EaseOut));

        ret.append(str(format("\t\tDeriv: %1%\n") % Deriv));

        ret.append(str(format("\t\tDerivIn:  (%1%, %2%, %3%)\n") % DerivInX % DerivInY % DerivInZ));
        ret.append(str(format("\t\tDerivOut: (%1%, %2%, %3%)\n") % DerivOutX % DerivOutY % DerivOutZ));

        ret.append(str(format("\t\tAngleKey: %1%\n") % AngleKey));
        // Wrong
        float comp = std::numeric_limits<float>::min();
        if (InterpolationX == 2) {
            if (std::abs(CPX1x) > comp || std::abs(CPX1y) > comp | std::abs(CPX2x) > comp || std::abs(CPX2y) > comp) {
                ret.append("Interpolation Type for X was 2, but CP were not!\n");
                valid = false;
            }
        }
        if (InterpolationY == 2) {
            if (std::abs(CPY1x) > comp || std::abs(CPY1y) > comp || std::abs(CPY2x) > comp || std::abs(CPY2y) > comp) {
                ret.append("Interpolation Type for Y was  2, but CP were not!\n");
                valid = false;
            }
        }
        if (InterpolationZ == 2) {
            if (std::abs(CPZ1x) > comp || std::abs(CPZ1y) > comp || std::abs(CPZ2x) > comp || std::abs(CPZ2y) > comp) {
                ret.append("Interpolation Type for Z was 2, but CP were not!\n");
                valid = false;
            }
        }


        if (Tens != 0) {
            ret.append("Unsupported parameter: Tens");
            valid = false;
        }

        if (Cont != 0) {
            ret.append("Unsupported parameter: Cont");
            valid = false;
        }

        if (Bias != 0) {
            ret.append("Unsupported parameter: Bias");
            valid = false;
        }

        if (EaseIn != 0) {
            ret.append("Unsupported parameter: EaseIn");
            valid = false;
        }

        if (EaseOut != 0) {
            ret.append("Unsupported parameter: EaseOut");
            valid = false;
        }

        if (Deriv != 0 || DerivInX != 0 || DerivInY != 0 || DerivInZ != 0 || DerivOutX != 0 || DerivOutY != 0 ||
            DerivOutZ != 0) {
            ret.append("Unsupported parameter(s): Deriv...");
            valid = false;
        }

        if (!valid) {
            throw std::runtime_error(ret);
        }
        return ret;
    }

    bool Keyframe::checkInterpolationType(InterpolationType type) {

        if (type == INTERPOLATION_TCB) {

            std::cerr << "Warning, TCB Interpolation is not handled, but is valid for convenience." << std::endl;
            return true;
        }
        return (type == INTERPOLATION_STEP || type == INTERPOLATION_BEZIER || type == INTERPOLATION_LINEAR);
    }

    std::string Keyframe::getInterpolationTypeName(InterpolationType type) {
        if (type > INTERPOLATION_TCB) {
            throw std::runtime_error("Type name not in enumeration");
        } else {
            const std::string InterpolationTypeName[] = {"UNKNOWN", "STEP", "LINEAR", "QUADRATIC", "CUBIC", "BEZIER",
                                                         "BEZIER_LINEARTIME", "TCB"};
            return InterpolationTypeName[type];
        }
    }

    float Keyframe::getValueByAxis(const std::string &axis) {
        if (axis == "X") {
            return ValueX;
        } else if (axis == "Y") {
            return ValueY;
        } else if (axis == "Z") {
            return ValueZ;
        } else {
            throw std::runtime_error("Invalid axis!");
        }

    }

    InterpolationType Keyframe::getInterpByAxis(const std::string &axis) {
        if (axis == "X") {
            return InterpolationX;
        } else if (axis == "Y") {
            return InterpolationY;
        } else if (axis == "Z") {
            return InterpolationZ;
        } else {
            throw std::runtime_error("Invalid axis!");
        }

    }


    void Keyframe::WriteChannel(pugi::xml_node node, std::string &axis) {
        InterpolationType interp = getInterpByAxis(axis);
        if (!checkInterpolationType(interp) || interp == INTERPOLATION_TCB) {
            throw std::runtime_error("Cannot write keyframe");
        }
        // TODO verify assumptions about framerate/encode them into a check
        int frameNum = numeric_cast<int>(30.0 * Time);
        auto tgtNode = node.append_child("frame");
        tgtNode.append_attribute("id").set_value(frameNum);
        auto interpStr = getInterpolationTypeName(interp);
        auto value = getValueByAxis(axis);
        tgtNode.append_attribute("value").set_value(formatFloat(value).c_str());
        WriteHandle(tgtNode, axis, false);
        WriteHandle(tgtNode, axis, true);
        tgtNode.append_attribute("interpolation").set_value(interpStr.c_str());

    }

    void Keyframe::WriteHandle(pugi::xml_node node, std::string &axis, bool right) {

        pugi::xml_node tgtNode;
        if (right) {
            tgtNode = node.append_child("handle_left");
        } else {
            tgtNode = node.append_child("handle_right");
        }
        auto handle = getControlPoint(axis, right);
        tgtNode.append_attribute("X").set_value(formatFloat(handle.first).c_str());
        tgtNode.append_attribute("Y").set_value(formatFloat(handle.second).c_str());
    }

    std::pair<float, float> Keyframe::getControlPoint(const std::string &axis, bool right) {

        if (!right) {
            if (axis == "X") {
                return std::make_pair(CPX1x, CPX1y);
            } else if (axis == "Y") {
                return std::make_pair(CPY1x, CPY1y);
            } else if (axis == "Z") {
                return std::make_pair(CPZ1x, CPZ1y);
            } else {
                throw std::runtime_error("Invalid axis!");
            }
        } else {
            if (axis == "X") {
                return std::make_pair(CPX2x, CPX2y);
            } else if (axis == "Y") {
                return std::make_pair(CPY2x, CPY2y);
            } else if (axis == "Z") {
                return std::make_pair(CPZ2x, CPZ2y);
            } else {
                throw std::runtime_error("Invalid axis!");
            }
        }
    }

    std::string Keyframe::formatFloat(float f) {
        return str(format("%.8e") % f);
    }
}