#include <X4ConverterTools/Ani/AniKeyframe.h>

using namespace boost;
using namespace Assimp;

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

AniKeyframe::AniKeyframe(StreamReader<> &reader) {
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

std::string AniKeyframe::validate() {
    bool valid = true;
    std::string ret = "\tKeyFrame: \n";

    ret.append(str(format("\t\tValue: (%1%, %2%, %3%)\n") % ValueX % ValueY % ValueZ));

    ret.append(
            str(format("\t\tInterpolation Types: (%1%, %2%, %3%)\n") % InterpolationX % InterpolationY %
                InterpolationZ));


    // TODO something better
    if (InterpolationX != 1 && InterpolationX != 2 && InterpolationX != 5 && InterpolationY != 7) {
        ret.append("\t\t New Interpolation type!");
        valid = false;
    }
    if (InterpolationY != 1 && InterpolationY != 2 && InterpolationY != 5 && InterpolationY != 7) {
        ret.append("\t\t New Interpolation type!");
        valid = false;
    }
    if (InterpolationZ != 1 && InterpolationZ != 2 && InterpolationZ != 5 && InterpolationZ != 7) {
        ret.append("\t\t New Interpolation type!");
        valid = false;
    }


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
    if ( InterpolationY == 2) {
        if (std::abs(CPY1x) > comp || std::abs(CPY1y) > comp || std::abs(CPY2x) > comp || std::abs(CPY2y) > comp) {
            ret.append("Interpolation Type for Y was  2, but CP were not!\n");
            valid = false;
        }
    }
    if ( InterpolationZ == 2) {
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