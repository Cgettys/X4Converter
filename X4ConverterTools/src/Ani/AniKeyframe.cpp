#include <X4ConverterTools/Ani/AniKeyframe.h>

using namespace boost;
using namespace Assimp;

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
// // Wrong
//    if (InterpolationX == 2) {
//        if (CPX1x != 0 || CPX1y != 0 || CPX2x != 0 || CPX2y != 0) {
//            ret.append("Interpolation Type for X was 1 or 2, but CP were not!\n");
//            valid = false;
//        }
//    }
//    if (InterpolationY == 2) {
//        if (CPY1x != 0 || CPY1y != 0 || CPY2x != 0 || CPY2y != 0) {
//            ret.append("Interpolation Type for Y was 1 or 2, but CP were not!\n");
//            valid = false;
//        }
//    }
//    if (InterpolationZ == 2) {
//        if (CPZ1x != 0 || CPZ1y != 0 || CPZ2x != 0 || CPZ2y != 0) {
//            ret.append("Interpolation Type for Z was 1 or 2, but CP were not!\n");
//            valid = false;
//        }
//    }


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