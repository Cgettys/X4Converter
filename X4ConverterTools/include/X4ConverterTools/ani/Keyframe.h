#pragma once

#include <assimp/StreamReader.h>
#include <boost/format.hpp>
#include <iostream>
#include <exception>
#include <cmath>
#include <limits.h>
#include <utility>
namespace ani {
    enum InterpolationType {
        INTERPOLATION_UNKNOWN,                /**< Unknown interpolation. */
        INTERPOLATION_STEP,                    /**< No interpolation. */
        INTERPOLATION_LINEAR,                /**< Linear interpolation.*/
        INTERPOLATION_QUADRATIC,
        INTERPOLATION_CUBIC,
        INTERPOLATION_BEZIER,                /**< Bezier interpolation with interpolated time. */
        INTERPOLATION_BEZIER_LINEARTIME,    /**< Bezier interpolation with linear time. */
        INTERPOLATION_TCB                    /**< TCB interpolation (using tension, continuity and bias generated hermite tangents). */
    };

    class Keyframe {
    public:
        Keyframe() = default;

        explicit Keyframe(Assimp::StreamReaderLE &reader);

        // TODO tuple everything?
        float getValueByAxis(std::string axis);

        InterpolationType getInterpByAxis(std::string axis);

        std::pair<float, float> getControlPoint(std::string axis, bool in);

        std::string validate();// Debug method - throws exception if invalid, else returns human readable string
        static std::string getInterpolationTypeName(InterpolationType type);

        static bool checkInterpolationType(InterpolationType type);

        float getTime() const;

    protected:
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

        float Tens;                                            /**< Tension. 4*/
        float Cont;                                            /**< Continuity. 8*/
        float Bias;                                            /**< Bias. 12*/
        float EaseIn;                                        /**< Ease In. 16*/
        float EaseOut;                                        /**< Ease Out. 20*/
        int Deriv;                                            /**< 24 Indicates whether derivatives have been calculated already. Is mutable to allow it being altered in the CalculateDerivatives() method. */
        float DerivInX, DerivInY, DerivInZ;                    /**< 12 Derivative In value. Is mutable to allow it being altered in the CalculateDerivatives() method. */
        float DerivOutX, DerivOutY, DerivOutZ;                /**< 24 Derivative Out value.  Is mutable to allow it being altered in the CalculateDerivatives() method.*/
        uint32_t AngleKey;                                        /** 28		// this will be set to non null if there is a key */

    private:
    };
}