#include <X4ConverterTools/util/DXUtil.h>

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

    aiVector3D DXUtil::ConvertVertexAttributeToAiVector3D(uint8_t *pAttribute, D3DDECLTYPE type) {
        switch (type) {
            case D3DDECLTYPE_FLOAT1:
                return aiVector3D(((float *) pAttribute)[0], 0.0f, 0.0f);

            case D3DDECLTYPE_FLOAT2:
                return aiVector3D(((float *) pAttribute)[0], ((float *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_FLOAT3:
            case D3DDECLTYPE_FLOAT4:
                return aiVector3D(((float *) pAttribute)[0], ((float *) pAttribute)[1], ((float *) pAttribute)[2]);

            case D3DDECLTYPE_D3DCOLOR:
                return aiVector3D(((float) pAttribute[2] / 255.0f) * 2.0f - 1.0f,
                                  ((float) pAttribute[1] / 255.0f) * 2.0f - 1.0f,
                                  ((float) pAttribute[0] / 255.0f) * 2.0f - 1.0f);

            case D3DDECLTYPE_SHORT2:
                return aiVector3D(((short *) pAttribute)[0], ((short *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_SHORT4:
                return aiVector3D(((short *) pAttribute)[0], ((short *) pAttribute)[1], ((short *) pAttribute)[2]);

            case D3DDECLTYPE_SHORT2N:
                return aiVector3D((float) ((short *) pAttribute)[0] / 32767.0f,
                                  (float) ((short *) pAttribute)[1] / 32767.0f,
                                  0.0f);

            case D3DDECLTYPE_SHORT4N:
                return aiVector3D((float) ((short *) pAttribute)[0] / 32767.0f,
                                  (float) ((short *) pAttribute)[1] / 32767.0f,
                                  (float) ((short *) pAttribute)[2] / 32767.0f);

            case D3DDECLTYPE_USHORT2N:
                return aiVector3D((float) ((uint16_t *) pAttribute)[0] / 65535.0f,
                                  (float) ((uint16_t *) pAttribute)[1] / 65535.0f,
                                  0.0f);

            case D3DDECLTYPE_USHORT4N:
                return aiVector3D((float) ((uint16_t *) pAttribute)[0] / 65535.0f,
                                  (float) ((uint16_t *) pAttribute)[1] / 65535.0f,
                                  (float) ((uint16_t *) pAttribute)[2] / 65535.0f);

            case D3DDECLTYPE_FLOAT16_2:
                return aiVector3D(((half_float::half *) pAttribute)[0], ((half_float::half *) pAttribute)[1], 0.0f);

            case D3DDECLTYPE_FLOAT16_4:
                return aiVector3D(((half_float::half *) pAttribute)[0], ((half_float::half *) pAttribute)[1],
                                  ((half_float::half *) pAttribute)[2]);

            default:
                throw std::runtime_error("Unsupported vertex element type for vectors");
        }
    }

    aiColor4D DXUtil::ConvertVertexAttributeToColorF(uint8_t *pAttribute, D3DDECLTYPE type) {
        switch (type) {
            case D3DDECLTYPE_FLOAT3:
                return aiColor4D(((float *) pAttribute)[0],
                                 ((float *) pAttribute)[1],
                                 ((float *) pAttribute)[2],
                                 1.0f);

            case D3DDECLTYPE_FLOAT4:
                return aiColor4D(((float *) pAttribute)[0],
                                 ((float *) pAttribute)[1],
                                 ((float *) pAttribute)[2],
                                 ((float *) pAttribute)[3]);

            case D3DDECLTYPE_FLOAT16_4:
                return aiColor4D(((half_float::half *) pAttribute)[0],
                                 ((half_float::half *) pAttribute)[1],
                                 ((half_float::half *) pAttribute)[2],
                                 ((half_float::half *) pAttribute)[3]);

            case D3DDECLTYPE_D3DCOLOR:
                return aiColor4D((float) pAttribute[2] / 255.0f,
                                 (float) pAttribute[1] / 255.0f,
                                 (float) pAttribute[0] / 255.0f,
                                 (float) pAttribute[3] / 255.0f);

            default:
                throw std::runtime_error("Unsupported vertex element type for colors");
        }
    }

    int DXUtil::WriteAiVector3DToVertexAttribute(aiVector3D vector, D3DDECLTYPE type, uint8_t *pAttribute) {
        switch (type) {
            case D3DDECLTYPE_FLOAT1:
                ((float *) pAttribute)[0] = vector.x;
                break;

            case D3DDECLTYPE_FLOAT2:
                ((float *) pAttribute)[0] = vector.x;
                ((float *) pAttribute)[1] = vector.y;
                break;

            case D3DDECLTYPE_FLOAT3:
                ((float *) pAttribute)[0] = vector.x;
                ((float *) pAttribute)[1] = vector.y;
                ((float *) pAttribute)[2] = vector.z;
                break;

            case D3DDECLTYPE_FLOAT4:
                ((float *) pAttribute)[0] = vector.x;
                ((float *) pAttribute)[1] = vector.y;
                ((float *) pAttribute)[2] = vector.z;
                ((float *) pAttribute)[3] = 0.0f;
                break;

            case D3DDECLTYPE_D3DCOLOR:
                pAttribute[2] = (uint8_t) ((vector.x + 1.0f) / 2.0f * 255.0f);
                pAttribute[1] = (uint8_t) ((vector.y + 1.0f) / 2.0f * 255.0f);
                pAttribute[0] = (uint8_t) ((vector.z + 1.0f) / 2.0f * 255.0f);
                pAttribute[3] = 0;
                break;

            case D3DDECLTYPE_SHORT2:
                ((short *) pAttribute)[0] = (short) vector.x;
                ((short *) pAttribute)[1] = (short) vector.y;
                break;

            case D3DDECLTYPE_SHORT4:
                ((short *) pAttribute)[0] = (short) vector.x;
                ((short *) pAttribute)[1] = (short) vector.y;
                ((short *) pAttribute)[2] = (short) vector.z;
                ((short *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_SHORT2N:
                ((short *) pAttribute)[0] = (short) (vector.x * 32767.0f);
                ((short *) pAttribute)[1] = (short) (vector.y * 32767.0f);
                break;

            case D3DDECLTYPE_SHORT4N:
                ((short *) pAttribute)[0] = (short) (vector.x * 32767.0f);
                ((short *) pAttribute)[1] = (short) (vector.y * 32767.0f);
                ((short *) pAttribute)[2] = (short) (vector.z * 32767.0f);
                ((short *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_USHORT2N:
                ((uint16_t *) pAttribute)[0] = (uint16_t) (vector.x * 65535.0f);
                ((uint16_t *) pAttribute)[1] = (uint16_t) (vector.y * 65535.0f);
                break;

            case D3DDECLTYPE_USHORT4N:
                ((uint16_t *) pAttribute)[0] = (uint16_t) (vector.x * 65535.0f);
                ((uint16_t *) pAttribute)[1] = (uint16_t) (vector.y * 65535.0f);
                ((uint16_t *) pAttribute)[2] = (uint16_t) (vector.z * 65535.0f);
                ((uint16_t *) pAttribute)[3] = 0;
                break;

            case D3DDECLTYPE_FLOAT16_2:
                ((half_float::half *) pAttribute)[0] = (half_float::half) vector.x;
                ((half_float::half *) pAttribute)[1] = (half_float::half) vector.y;
                break;

            case D3DDECLTYPE_FLOAT16_4:
                ((half_float::half *) pAttribute)[0] = (half_float::half) vector.x;
                ((half_float::half *) pAttribute)[1] = (half_float::half) vector.y;
                ((half_float::half *) pAttribute)[2] = (half_float::half) vector.z;
                ((half_float::half *) pAttribute)[3] = (half_float::half) 0.0f;
                break;

            default:
                throw std::runtime_error("Unsupported vertex element type for vectors");
        }
        return GetVertexElementTypeSize(type);
    }

    int DXUtil::WriteColorFToVertexAttribute(const aiColor4D &color, D3DDECLTYPE type, uint8_t *pAttribute) {
        switch (type) {
            case D3DDECLTYPE_FLOAT3:
                ((float *) pAttribute)[0] = color.r;
                ((float *) pAttribute)[1] = color.g;
                ((float *) pAttribute)[2] = color.b;
                break;

            case D3DDECLTYPE_FLOAT4:
                ((float *) pAttribute)[0] = color.r;
                ((float *) pAttribute)[1] = color.g;
                ((float *) pAttribute)[2] = color.b;
                ((float *) pAttribute)[3] = color.a;
                break;

            case D3DDECLTYPE_FLOAT16_4:
                ((half_float::half *) pAttribute)[0] = (half_float::half) color.r;
                ((half_float::half *) pAttribute)[1] = (half_float::half) color.g;
                ((half_float::half *) pAttribute)[2] = (half_float::half) color.b;
                ((half_float::half *) pAttribute)[3] = (half_float::half) color.a;
                break;

            case D3DDECLTYPE_D3DCOLOR:
                pAttribute[2] = (uint8_t) (color.r * 255.0f);
                pAttribute[1] = (uint8_t) (color.g * 255.0f);
                pAttribute[0] = (uint8_t) (color.b * 255.0f);
                pAttribute[3] = (uint8_t) (color.a * 255.0f);
                break;

            default:
                throw std::runtime_error("Unsupported vertex element type for colors");
        }
        return GetVertexElementTypeSize(type);
    }
}