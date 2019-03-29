#include <X4ConverterTools/util/DXUtil.h>
#include "half.hpp"
namespace util {
    int DXUtil::GetVertexElementTypeSize(D3DDECLTYPE type) {
        switch (type) {
            case D3DDECLTYPE_FLOAT1:
            case D3DDECLTYPE_D3DCOLOR:
            case D3DDECLTYPE_UBYTE4:
            case D3DDECLTYPE_UBYTE4N:
            case D3DDECLTYPE_SHORT2:
            case D3DDECLTYPE_SHORT2N:
            case D3DDECLTYPE_USHORT2N:
            case D3DDECLTYPE_FLOAT16_2:
            case D3DDECLTYPE_DEC3N:
            case D3DDECLTYPE_UDEC3:
                return 4;

            case D3DDECLTYPE_FLOAT2:
            case D3DDECLTYPE_SHORT4:
            case D3DDECLTYPE_SHORT4N:
            case D3DDECLTYPE_USHORT4N:
            case D3DDECLTYPE_FLOAT16_4:
                return 8;

            case D3DDECLTYPE_FLOAT3:
                return 12;

            case D3DDECLTYPE_FLOAT4:
                return 16;
            default:
                throw std::runtime_error("Unknown vertex element type");
        }
    }

    Vec3D DXUtil::ConvertVertexAttributeToVec3D(uint8_t *pAttribute, D3DDECLTYPE type) {
        switch (type) {
            case D3DDECLTYPE_FLOAT1:
                return Vec3D(((float *) pAttribute)[0], 0.0f, 0.0f);

            case D3DDECLTYPE_FLOAT2:
                return Vec3D(((float *) pAttribute)[0], ((float *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_FLOAT3:
            case D3DDECLTYPE_FLOAT4:
                return Vec3D(((float *) pAttribute)[0], ((float *) pAttribute)[1], ((float *) pAttribute)[2]);

            case D3DDECLTYPE_D3DCOLOR:
                return Vec3D(((float) pAttribute[2] / 255.0f) * 2.0f - 1.0f,
                             ((float) pAttribute[1] / 255.0f) * 2.0f - 1.0f,
                             ((float) pAttribute[0] / 255.0f) * 2.0f - 1.0f);

            case D3DDECLTYPE_SHORT2:
                return Vec3D(((short *) pAttribute)[0], ((short *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_SHORT4:
                return Vec3D(((short *) pAttribute)[0], ((short *) pAttribute)[1], ((short *) pAttribute)[2]);

            case D3DDECLTYPE_SHORT2N:
                return Vec3D((float) ((short *) pAttribute)[0] / 32767.0f, (float) ((short *) pAttribute)[1] / 32767.0f,
                             0.0f);

            case D3DDECLTYPE_SHORT4N:
                return Vec3D((float) ((short *) pAttribute)[0] / 32767.0f, (float) ((short *) pAttribute)[1] / 32767.0f,
                             (float) ((short *) pAttribute)[2] / 32767.0f);

            case D3DDECLTYPE_USHORT2N:
                return Vec3D((float) ((uint16_t *) pAttribute)[0] / 65535.0f,
                             (float) ((uint16_t *) pAttribute)[1] / 65535.0f,
                             0.0f);

            case D3DDECLTYPE_USHORT4N:
                return Vec3D((float) ((uint16_t *) pAttribute)[0] / 65535.0f,
                             (float) ((uint16_t *) pAttribute)[1] / 65535.0f,
                             (float) ((uint16_t *) pAttribute)[2] / 65535.0f);

            case D3DDECLTYPE_FLOAT16_2:
                return Vec3D(((half_float::half *) pAttribute)[0], ((half_float::half *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_FLOAT16_4:
                return Vec3D(((half_float::half *) pAttribute)[0], ((half_float::half *) pAttribute)[1],
                             ((half_float::half *) pAttribute)[2]);

            default:
                throw std::runtime_error("Unsupported vertex element type for vectors");
        }
    }

    aiColor4D DXUtil::ConvertVertexAttributeToColorF(uint8_t *pAttribute, D3DDECLTYPE type) {
        switch (type) {
            case D3DDECLTYPE_FLOAT3:
                return (aiColor4D) Color(((float *) pAttribute)[0], ((float *) pAttribute)[1],
                                         ((float *) pAttribute)[2],
                                         1.0f);

            case D3DDECLTYPE_FLOAT4:
                return (aiColor4D) Color(((float *) pAttribute)[0], ((float *) pAttribute)[1],
                                         ((float *) pAttribute)[2],
                                         ((float *) pAttribute)[3]);

            case D3DDECLTYPE_FLOAT16_4:
                return (aiColor4D) Color(((half_float::half *) pAttribute)[0], ((half_float::half *) pAttribute)[1],
                                         ((half_float::half *) pAttribute)[2], ((half_float::half *) pAttribute)[3]);

            case D3DDECLTYPE_D3DCOLOR:
                return (aiColor4D) Color((float) pAttribute[2] / 255.0f, (float) pAttribute[1] / 255.0f,
                                         (float) pAttribute[0] / 255.0f, (float) pAttribute[3] / 255.0f);

            default:
                throw std::runtime_error("Unsupported vertex element type for colors");
        }
    }

    int DXUtil::WriteVec3DToVertexAttribute(aiVector3D vec, D3DDECLTYPE type, uint8_t *pAttribute) {
        const Vec3D vector = (Vec3D) vec;
        switch (type) {
            case D3DDECLTYPE_FLOAT1:
                ((float *) pAttribute)[0] = vector.X;
                break;

            case D3DDECLTYPE_FLOAT2:
                ((float *) pAttribute)[0] = vector.X;
                ((float *) pAttribute)[1] = vector.Y;
                break;

            case D3DDECLTYPE_FLOAT3:
                ((float *) pAttribute)[0] = vector.X;
                ((float *) pAttribute)[1] = vector.Y;
                ((float *) pAttribute)[2] = vector.Z;
                break;

            case D3DDECLTYPE_FLOAT4:
                ((float *) pAttribute)[0] = vector.X;
                ((float *) pAttribute)[1] = vector.Y;
                ((float *) pAttribute)[2] = vector.Z;
                ((float *) pAttribute)[3] = 0.0f;
                break;

            case D3DDECLTYPE_D3DCOLOR:
                pAttribute[2] = (uint8_t) ((vector.X + 1.0f) / 2.0f * 255.0f);
                pAttribute[1] = (uint8_t) ((vector.Y + 1.0f) / 2.0f * 255.0f);
                pAttribute[0] = (uint8_t) ((vector.Z + 1.0f) / 2.0f * 255.0f);
                pAttribute[3] = 0;
                break;

            case D3DDECLTYPE_SHORT2:
                ((short *) pAttribute)[0] = (short) vector.X;
                ((short *) pAttribute)[1] = (short) vector.Y;
                break;

            case D3DDECLTYPE_SHORT4:
                ((short *) pAttribute)[0] = (short) vector.X;
                ((short *) pAttribute)[1] = (short) vector.Y;
                ((short *) pAttribute)[2] = (short) vector.Z;
                ((short *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_SHORT2N:
                ((short *) pAttribute)[0] = (short) (vector.X * 32767.0f);
                ((short *) pAttribute)[1] = (short) (vector.Y * 32767.0f);
                break;

            case D3DDECLTYPE_SHORT4N:
                ((short *) pAttribute)[0] = (short) (vector.X * 32767.0f);
                ((short *) pAttribute)[1] = (short) (vector.Y * 32767.0f);
                ((short *) pAttribute)[2] = (short) (vector.Z * 32767.0f);
                ((short *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_USHORT2N:
                ((uint16_t *) pAttribute)[0] = (uint16_t) (vector.X * 65535.0f);
                ((uint16_t *) pAttribute)[1] = (uint16_t) (vector.Y * 65535.0f);
                break;

            case D3DDECLTYPE_USHORT4N:
                ((uint16_t *) pAttribute)[0] = (uint16_t) (vector.X * 65535.0f);
                ((uint16_t *) pAttribute)[1] = (uint16_t) (vector.Y * 65535.0f);
                ((uint16_t *) pAttribute)[2] = (uint16_t) (vector.Z * 65535.0f);
                ((uint16_t *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_FLOAT16_2:
                ((half_float::half *) pAttribute)[0] = (half_float::half) vector.X;
                ((half_float::half *) pAttribute)[1] = (half_float::half) vector.Y;
                break;

            case D3DDECLTYPE_FLOAT16_4:
                ((half_float::half *) pAttribute)[0] = (half_float::half) vector.X;
                ((half_float::half *) pAttribute)[1] = (half_float::half) vector.Y;
                ((half_float::half *) pAttribute)[2] = (half_float::half) vector.Z;
                ((half_float::half *) pAttribute)[3] = (half_float::half) 0.0f;
                break;

            default:
                throw std::runtime_error("Unsupported vertex element type for vectors");
        }
        return GetVertexElementTypeSize(type);
    }

    int DXUtil::WriteColorFToVertexAttribute(const aiColor4D &aiColor, D3DDECLTYPE type, uint8_t *pAttribute) {
        const Color color = (Color) aiColor;
        switch (type) {
            case D3DDECLTYPE_FLOAT3:
                ((float *) pAttribute)[0] = color.R;
                ((float *) pAttribute)[1] = color.G;
                ((float *) pAttribute)[2] = color.B;
                break;

            case D3DDECLTYPE_FLOAT4:
                ((float *) pAttribute)[0] = color.R;
                ((float *) pAttribute)[1] = color.G;
                ((float *) pAttribute)[2] = color.B;
                ((float *) pAttribute)[3] = color.A;
                break;

            case D3DDECLTYPE_FLOAT16_4:
                ((half_float::half *) pAttribute)[0] = (half_float::half) color.R;
                ((half_float::half *) pAttribute)[1] = (half_float::half) color.G;
                ((half_float::half *) pAttribute)[2] = (half_float::half) color.B;
                ((half_float::half *) pAttribute)[3] = (half_float::half) color.A;
                break;

            case D3DDECLTYPE_D3DCOLOR:
                pAttribute[2] = (uint8_t) (color.R * 255.0f);
                pAttribute[1] = (uint8_t) (color.G * 255.0f);
                pAttribute[0] = (uint8_t) (color.B * 255.0f);
                pAttribute[3] = (uint8_t) (color.A * 255.0f);
                break;

            default:
                throw std::runtime_error("Unsupported vertex element type for colors");
        }
        return GetVertexElementTypeSize(type);
    }
}