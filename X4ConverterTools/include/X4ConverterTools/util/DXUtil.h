#pragma once

#include <X4ConverterTools/types.h>
#include <assimp/types.h>
#include "../Vendor/half/include/half.hpp"
#include <boost/format.hpp>
#include <cstdint>
namespace util {
class DXUtil {
 public:
  static int GetVertexElementTypeSize(D3DDECLTYPE type);

  static aiVector3D ConvertVertexAttributeToAiVector3D(uint8_t *pAttribute, D3DDECLTYPE type);

  static aiColor4D ConvertVertexAttributeToColorF(uint8_t *pAttribute, D3DDECLTYPE type);

  static int WriteAiVector3DToVertexAttribute(aiVector3D vector, D3DDECLTYPE type, uint8_t *pAttribute);

  static int WriteColorFToVertexAttribute(const aiColor4D &color, D3DDECLTYPE type, uint8_t *pAttribute);
};
}