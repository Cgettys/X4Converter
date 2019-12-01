#include <X4ConverterTools/util/DXUtil.h>
#include <boost/numeric/conversion/cast.hpp>
namespace util {
using boost::numeric_cast;
static float kUINT8Scale = 255.0f;
static float kINT16Scale = 32767.0f;
static float kUINT16Scale = 65535.0f;

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
  auto *pFP = reinterpret_cast<float *>(pAttribute);
  auto *pHFP = reinterpret_cast<half_float::half *>(pAttribute);
  auto *pINT16 = reinterpret_cast<int16_t *>(pAttribute);
  auto *pUINT16 = reinterpret_cast<int16_t *>(pAttribute);
  switch (type) {
    case D3DDECLTYPE_FLOAT1:
      return aiVector3D(pFP[0], 0.0f, 0.0f);

    case D3DDECLTYPE_FLOAT2:
      return aiVector3D(pFP[0], pFP[1], 0.0f);

    case D3DDECLTYPE_FLOAT3:
    case D3DDECLTYPE_FLOAT4:
      // TODO do we see D3DDECLTYPE_FLOAT4?
      return aiVector3D(pFP[0], pFP[1], pFP[2]);

    case D3DDECLTYPE_D3DCOLOR:
      return aiVector3D(pAttribute[2], pAttribute[1], pAttribute[0]) / kUINT8Scale;

    case D3DDECLTYPE_SHORT2:
      return aiVector3D(pINT16[0], pINT16[1], 0.0f);

    case D3DDECLTYPE_SHORT4:
      return aiVector3D(pINT16[0], pINT16[1], pINT16[2]);

    case D3DDECLTYPE_SHORT2N:
      return aiVector3D(pINT16[0] / kINT16Scale, pINT16[1] / kINT16Scale, 0.0f);

    case D3DDECLTYPE_SHORT4N:
      return aiVector3D(pINT16[0] / kINT16Scale, pINT16[1] / kINT16Scale, pINT16[2] / kINT16Scale);

    case D3DDECLTYPE_USHORT2N:
      return aiVector3D(pUINT16[0] / kUINT16Scale, pUINT16[1] / kUINT16Scale, 0.0f);

    case D3DDECLTYPE_USHORT4N:
      return aiVector3D(pUINT16[0] / kUINT16Scale, pUINT16[1] / kUINT16Scale, pUINT16[2] / kUINT16Scale);

    case D3DDECLTYPE_FLOAT16_2:
      return aiVector3D(pHFP[0], pHFP[1], 0.0f);

    case D3DDECLTYPE_FLOAT16_4:
      // TODO missing 4
      return aiVector3D(pHFP[0], pHFP[1], pHFP[2]);

    default:
      throw std::runtime_error("Unsupported vertex element type for vectors");
  }
}

aiColor4D DXUtil::ConvertVertexAttributeToColorF(uint8_t *pAttr, D3DDECLTYPE type) {
  auto *pFP = reinterpret_cast<float *>(pAttr);
  auto *pHFP = reinterpret_cast<half_float::half *>(pAttr);
  switch (type) {
    case D3DDECLTYPE_FLOAT3:
      return aiColor4D(pFP[0], pFP[1], pFP[2], 1.0f);
    case D3DDECLTYPE_FLOAT4:
      return aiColor4D(pFP[0], pFP[1], pFP[2], pFP[3]);
    case D3DDECLTYPE_FLOAT16_4:
      return aiColor4D(pHFP[0], pHFP[1], pHFP[2], pHFP[3]);
    case D3DDECLTYPE_D3DCOLOR:
      return aiColor4D(pAttr[2] / kUINT8Scale, pAttr[1] / kUINT8Scale, pAttr[0] / kUINT8Scale, pAttr[3] / kUINT8Scale);
    default:
      throw std::runtime_error("Unsupported vertex element type for colors");
  }
}

int DXUtil::WriteAiVector3DToVertexAttribute(aiVector3D vector, D3DDECLTYPE type, uint8_t *pAttribute) {
  auto *pFP = reinterpret_cast<float *>(pAttribute);
  auto *pHFP = reinterpret_cast<half_float::half *>(pAttribute);
  auto *pINT16 = reinterpret_cast<int16_t *>(pAttribute);
  auto *pUINT16 = reinterpret_cast<int16_t *>(pAttribute);
  switch (type) {
    case D3DDECLTYPE_FLOAT1:
      pFP[0] = vector.x;
      break;
    case D3DDECLTYPE_FLOAT2:
      pFP[0] = vector.x;
      pFP[1] = vector.y;
      break;
    case D3DDECLTYPE_FLOAT3:
      pFP[0] = vector.x;
      pFP[1] = vector.y;
      pFP[2] = vector.z;
      break;
    case D3DDECLTYPE_FLOAT4:
      pFP[0] = vector.x;
      pFP[1] = vector.y;
      pFP[2] = vector.z;
      pFP[3] = 0.0f;
      break;

    case D3DDECLTYPE_D3DCOLOR:
      pAttribute[2] = numeric_cast<uint8_t>(vector.x * kUINT8Scale);
      pAttribute[1] = numeric_cast<uint8_t>(vector.y * kUINT8Scale);
      pAttribute[0] = numeric_cast<uint8_t>(vector.z * kUINT8Scale);
      pAttribute[3] = 0;
      break;

    case D3DDECLTYPE_SHORT2:
      pINT16[0] = numeric_cast<int16_t>(vector.x);
      pINT16[1] = numeric_cast<int16_t>(vector.y);
      break;

    case D3DDECLTYPE_SHORT4:
      pINT16[0] = numeric_cast<int16_t>(vector.x);
      pINT16[1] = numeric_cast<int16_t>(vector.y);
      pINT16[2] = numeric_cast<int16_t>(vector.z);
      pINT16[3] = 0;
      break;

    case D3DDECLTYPE_SHORT2N:
      pINT16[0] = numeric_cast<int16_t>(vector.x * kINT16Scale);
      pINT16[1] = numeric_cast<int16_t>(vector.y * kINT16Scale);
      break;

    case D3DDECLTYPE_SHORT4N:
      pINT16[0] = numeric_cast<int16_t>(vector.x * kINT16Scale);
      pINT16[1] = numeric_cast<int16_t>(vector.y * kINT16Scale);
      pINT16[2] = numeric_cast<int16_t>(vector.z * kINT16Scale);
      pINT16[3] = 0;
      break;

    case D3DDECLTYPE_USHORT2N:
      pUINT16[0] = numeric_cast<uint16_t>(vector.x * kUINT16Scale);
      pUINT16[1] = numeric_cast<uint16_t>(vector.y * kUINT16Scale);
      break;

    case D3DDECLTYPE_USHORT4N:
      pUINT16[0] = numeric_cast<uint16_t>(vector.x * kUINT16Scale);
      pUINT16[1] = numeric_cast<uint16_t>(vector.y * kUINT16Scale);
      pUINT16[2] = numeric_cast<uint16_t>(vector.z * kUINT16Scale);
      pUINT16[3] = 0;
      break;

    case D3DDECLTYPE_FLOAT16_2:
      pHFP[0] = half_float::half{vector.x};
      pHFP[1] = half_float::half{vector.y};
      break;

    case D3DDECLTYPE_FLOAT16_4:
      pHFP[0] = half_float::half{vector.x};
      pHFP[1] = half_float::half{vector.y};
      pHFP[2] = half_float::half{vector.z};
      pHFP[3] = half_float::half{0.0f};
      break;

    default:
      throw std::runtime_error("Unsupported vertex element type for vectors");
  }
  return GetVertexElementTypeSize(type);
}

int DXUtil::WriteColorFToVertexAttribute(const aiColor4D &color, D3DDECLTYPE type, uint8_t *pAttribute) {
  auto *pFP = reinterpret_cast<float *>(pAttribute);
  auto *pHFP = reinterpret_cast<half_float::half *>(pAttribute);
  switch (type) {
    case D3DDECLTYPE_FLOAT3:
      pFP[0] = color.r;
      pFP[1] = color.g;
      pFP[2] = color.b;
      break;

    case D3DDECLTYPE_FLOAT4:
      pFP[0] = color.r;
      pFP[1] = color.g;
      pFP[2] = color.b;
      pFP[3] = color.a;
      break;

    case D3DDECLTYPE_FLOAT16_4:
      pHFP[0] = half_float::half{color.r};
      pHFP[1] = half_float::half{color.g};
      pHFP[2] = half_float::half{color.b};
      pHFP[3] = half_float::half{color.a};
      break;

    case D3DDECLTYPE_D3DCOLOR:
      pAttribute[2] = numeric_cast<uint8_t>(color.r * kUINT8Scale);
      pAttribute[1] = numeric_cast<uint8_t>(color.g * kUINT8Scale);
      pAttribute[0] = numeric_cast<uint8_t>(color.b * kUINT8Scale);
      pAttribute[3] = numeric_cast<uint8_t>(color.a * kUINT8Scale);
      break;

    default:
      throw std::runtime_error("Unsupported vertex element type for colors");
  }
  return GetVertexElementTypeSize(type);
}
}