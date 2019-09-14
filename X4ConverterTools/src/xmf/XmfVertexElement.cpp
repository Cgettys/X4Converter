#include <X4ConverterTools/xmf/XmfVertexElement.h>

namespace xmf {
XmfVertexElement::XmfVertexElement() {
  Type = D3DDECLTYPE_UNUSED;
  Usage = 0;
  UsageIndex = 0;
}

XmfVertexElement::XmfVertexElement(D3DDECLTYPE type, D3DDECLUSAGE usage, uint8_t usageIndex) {
  Type = type;
  Usage = usage;
  UsageIndex = usageIndex;
}

XmfVertexElement::XmfVertexElement(Assimp::StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}
void XmfVertexElement::ReadBinary(Assimp::StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}
void XmfVertexElement::ReadBinaryImpl(Assimp::StreamReaderLE &reader) {
  reader >> Type;
  reader >> Usage;
  reader >> UsageIndex;
  reader >> _pad0[0] >> _pad0[1];
// TODO check padding and ask about it
//    if (_pad0[0]!=0 || _pad0[1] !=0){
//        std::cout << str(format("%1% %2%") % (int)_pad0[0] % (int)_pad0[1])<<std::endl;
//        throw std::runtime_error("_pad0 must be 0!");
//    }
}
void XmfVertexElement::WriteBinary(Assimp::StreamWriterLE &writer) const {
  writer << Type;
  writer << Usage;
  writer << UsageIndex;
  writer << _pad0[0] << _pad0[1];
}

// TODO get rid of duplication/unused code
int XmfVertexElement::WriteVertexElement(aiMesh *pMesh, uint32_t vertexIdx, uint8_t *pElemData) {
  auto type = (D3DDECLTYPE) Type;
  switch (Usage) {
    case D3DDECLUSAGE_POSITION: {
      aiVector3D position;
      if (pMesh->mVertices) {
        position = pMesh->mVertices[vertexIdx];
        position.x = -position.x;
      }

      return util::DXUtil::WriteAiVector3DToVertexAttribute(position, type, pElemData);
    }

    case D3DDECLUSAGE_NORMAL: {
      aiVector3D normal;
      if (pMesh->mNormals) {
        normal = pMesh->mNormals[vertexIdx];
        normal.x = -normal.x;
      }

      return util::DXUtil::WriteAiVector3DToVertexAttribute(normal, type, pElemData);
    }

    case D3DDECLUSAGE_TANGENT: {
      aiVector3D tangent;
      if (pMesh->mTangents) {
        tangent = pMesh->mTangents[vertexIdx];
        tangent.x = -tangent.x;
      }

      return util::DXUtil::WriteAiVector3DToVertexAttribute(tangent, type, pElemData);
    }

    case D3DDECLUSAGE_TEXCOORD: {
      aiVector3D texcoord;
      if (pMesh->mTextureCoords[UsageIndex]) {
        texcoord = pMesh->mTextureCoords[UsageIndex][vertexIdx];
        texcoord.y = 1.0f - texcoord.y;
      }

      return util::DXUtil::WriteAiVector3DToVertexAttribute(texcoord, type, pElemData);
    }

    case D3DDECLUSAGE_COLOR: {
      aiColor4D color;
      if (pMesh->mColors[UsageIndex])
        color = pMesh->mColors[UsageIndex][vertexIdx];

      return util::DXUtil::WriteColorFToVertexAttribute(color, type, pElemData);
    }
    default:throw std::runtime_error("Usage not recognized");
  }
}

}