#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>

#include <iostream>
#include <sstream>
#include <boost/numeric/conversion/cast.hpp>
using namespace Assimp;
using namespace boost;
using boost::numeric_cast;
namespace xmf {
void XmfDataBufferDesc::ReadBinaryImpl(Assimp::StreamReaderLE &reader) {
  reader >> Type;
  reader >> UsageIndex;
  reader >> DataOffset;
  reader >> Compressed;
  for (uint8_t &b : _pad0) {
    reader >> b;
  }
  reader >> Format;
  reader >> CompressedDataSize;
  reader >> NumItemsPerSection;
  reader >> ItemSize;
  reader >> NumSections;
  for (uint8_t &b : _pad1) {
    reader >> b;
  }
  reader >> NumVertexElements;
  for (XmfVertexElement &e : VertexElements) {
    e = XmfVertexElement(reader);
  }
  //TODO make this validation code?
//    for (byte &b : _pad0) {
//        if (b != 0) {
//            throw std::runtime_error("Non-zero pad byte in _pad0");
//        }
//    }
//
//    for (byte &b : _pad1) {
//        if (b != 0) {
//            std::cout << (int) b << endl;
//            throw std::runtime_error("Non-zero pad byte in _pad1");
//        }
//    }
  NormalizeVertexDeclaration();
  validate();
}
XmfDataBufferDesc::XmfDataBufferDesc(StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}
void XmfDataBufferDesc::ReadBinary(Assimp::StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}

bool XmfDataBufferDesc::IsCompressed() const {
  return Compressed != 0;
}

bool XmfDataBufferDesc::IsVertexBuffer() const {
  return !IsIndexBuffer();
}

bool XmfDataBufferDesc::IsIndexBuffer() const {
  return Type == 30;
}

void XmfDataBufferDesc::NormalizeVertexDeclaration() {
  if (!IsVertexBuffer() || NumVertexElements > 0)
    return;

  NumVertexElements = 1;
  VertexElements[0].Type = Format;
  switch (Type) {
    case 0:
    case 1:VertexElements[0].Usage = D3DDECLUSAGE_POSITION;
      break;

    case 2:
    case 3:VertexElements[0].Usage = D3DDECLUSAGE_NORMAL;
      break;

    case 4:VertexElements[0].Usage = D3DDECLUSAGE_TANGENT;
      break;

    case 5:VertexElements[0].Usage = D3DDECLUSAGE_BINORMAL;
      break;

    case 8:VertexElements[0].Usage = D3DDECLUSAGE_COLOR;
      break;

    case 20:VertexElements[0].Usage = D3DDECLUSAGE_PSIZE;
      break;

    default:VertexElements[0].Usage = D3DDECLUSAGE_TEXCOORD;
      break;
  }
  VertexElements[0].UsageIndex = numeric_cast<uint8_t>(UsageIndex);

  Type = 0;
  UsageIndex = 0;
  Format = 32;
}

void XmfDataBufferDesc::DenormalizeVertexDeclaration() {
  if (!IsVertexBuffer() || NumVertexElements != 1)
    return;

  Format = VertexElements[0].Type;
  switch (VertexElements[0].Usage) {
    case D3DDECLUSAGE_POSITION:Type = 0;
      break;

    case D3DDECLUSAGE_NORMAL:Type = 2;
      break;

    case D3DDECLUSAGE_TANGENT:Type = 4;
      break;

    case D3DDECLUSAGE_BINORMAL:Type = 5;
      break;

    case D3DDECLUSAGE_COLOR:Type = 8;
      break;

    case D3DDECLUSAGE_PSIZE:Type = 20;
      break;

    default:Type = 31;
      break;
  }
  UsageIndex = VertexElements[0].UsageIndex;
  NumVertexElements = 0;
}

int XmfDataBufferDesc::GetVertexDeclarationSize() {
  int size = 0;
  for (int i = 0; i < NumVertexElements; ++i) {
    size += util::DXUtil::GetVertexElementTypeSize((D3DDECLTYPE) VertexElements[i].Type);
  }
  return size;
}

D3DFORMAT XmfDataBufferDesc::GetIndexFormat() {
  if (!IsIndexBuffer()) {
    throw std::runtime_error("The data buffer is not an index buffer");
  }
  return Format == 30 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
}

void XmfDataBufferDesc::WriteBinary(Assimp::StreamWriterLE &writer) const {
  writer << Type;
  writer << UsageIndex;
  writer << DataOffset;
  writer << Compressed;
  for (auto &b : _pad0) {
    writer << b;
  }

  writer << Format;
  writer << CompressedDataSize;
  writer << NumItemsPerSection;
  writer << ItemSize;
  writer << NumSections;
  for (auto &b : _pad1) {
    writer << b;
  }
  writer << NumVertexElements;
  for (auto &e : VertexElements) {
    e.WriteBinary(writer);
  }
}

std::string XmfDataBufferDesc::validate() {
  using std::endl;
  bool valid = true;
  std::stringstream ss;
  ss << "DataBufferDesc:\n";

  ss << "\tType: " << Type << endl;
  ss << "\tUsageIndex: " << UsageIndex << endl;
  ss << "\tDataOffset: " << DataOffset << endl;
  ss << "\tCompressed: " << Compressed << endl;
  //TODO ret.append(str(format("(w*): %1%\n")%Pad));
  ss << "\tFormat: " << Format << endl;
  ss << "\tCompressedDataSize: " << CompressedDataSize << endl;
  ss << "\tNumItemsPerSection: " << NumItemsPerSection << endl;
  ss << "\tItemSize: " << ItemSize << endl;
  ss << "\tNumSections: " << NumSections << endl;
  // TODO more padding
  ss << "\tNumVertexElements: " << NumVertexElements << endl;
  // TODO Vertex elements
  if (NumSections != 1) {
    ss << "\tERROR: Unexpected number of sections (must be 1)" << endl;
    valid = false;
  }
  if (IsVertexBuffer() && ItemSize != GetVertexDeclarationSize()) {
    ss << "\tERROR: Item size for vertex buffer is incorrect" << endl;
    valid = false;
  } else if (IsIndexBuffer()) {
    D3DFORMAT format = GetIndexFormat();
    if ((format == D3DFMT_INDEX16 && ItemSize != sizeof(uint16_t)) ||
        (format == D3DFMT_INDEX32 && ItemSize != sizeof(uint32_t))) {
      ss << "\tERROR: Item size for index buffer is incorrect" << endl;
      valid = false;
    }
  }
  if (!valid) {
    throw std::runtime_error(ss.str());
  }
  return ss.str();
}
}