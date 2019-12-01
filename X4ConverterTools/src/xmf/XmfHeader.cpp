#include <X4ConverterTools/xmf/XmfHeader.h>

#include <sstream>
#include <iostream>
#include <X4ConverterTools/xmf/XmfMaterial.h>
#include <X4ConverterTools/xmf/XmfDataBufferDesc.h>
#include <X4ConverterTools/types/DirectX.h>
using namespace Assimp;
using namespace boost;

namespace xmf {
XmfHeader::XmfHeader(uint8_t numDataBuffers, uint8_t numMaterials) {
// Note that this ignores any header read into this mesh file since we can't guarantee the old header is accurate
  Magic[0] = 'X';
  Magic[1] = 'U';
  Magic[2] = 'M';
  Magic[3] = 'F';
  Version = 3;
  IsBigEndian = static_cast<uint8_t>(false);
  SizeOfHeader = XmfHeader::kExpectedBinarySize;
  reserved0 = 0x00;
  NumDataBuffers = numDataBuffers;
  DataBufferDescSize = XmfDataBufferDesc::kExpectedBinarySize;
  NumMaterials = numMaterials;
  MaterialSize = XmfMaterial::kExpectedBinarySize;

  Culling_CW = static_cast<uint8_t>(false);
  RightHand = static_cast<uint8_t>(false);
  NumVertices = 0;//TODO
  NumIndices = 0;//TODO


  PrimitiveType = D3DPT_TRIANGLELIST;
  for (uint8_t &c : pad) {
    c = 0x00;
  }
  // TODO verify this
  MeshOptimization = 0;

}

XmfHeader::XmfHeader(Assimp::StreamReaderLE &reader) {
  for (uint8_t &c : Magic) {
    reader >> c;
  }

  reader >> Version;
  reader >> IsBigEndian;
  reader >> SizeOfHeader;
  reader >> reserved0;
  reader >> NumDataBuffers;
  reader >> DataBufferDescSize;
  reader >> NumMaterials;
  reader >> MaterialSize;

  reader >> Culling_CW;
  reader >> RightHand;
  reader >> NumVertices;
  reader >> NumIndices;
  reader >> PrimitiveType;
  reader >> MeshOptimization;

  for (float &f: BoundingBoxCenter) {
    reader >> f;
  }

  for (float &f: BoundingBoxSize) {
    reader >> f;
  }

  for (uint8_t &c : pad) {
    reader >> c;
  }
}

void XmfHeader::Write(Assimp::StreamWriterLE &writer) {
  for (uint8_t &c : Magic) {
    writer << c;
  }
  writer << Version << IsBigEndian;
  writer << SizeOfHeader;
  writer << reserved0;
  writer << NumDataBuffers << DataBufferDescSize;
  writer << NumMaterials << MaterialSize;

  writer << Culling_CW << RightHand;
  writer << NumVertices << NumIndices;

  writer << PrimitiveType << MeshOptimization;

  for (float &f: BoundingBoxCenter) {
    writer << f;
  }

  for (float &f: BoundingBoxSize) {
    writer << f;
  }

  for (uint8_t &c : pad) {
    writer << c;
  }
}

std::string XmfHeader::validate() const {
  using std::endl;
  std::stringstream ss;
  bool valid = true;

  // TODO prettyprinter

  if (memcmp(Magic, "XUMF", 4) != 0) {
    ss << "\tERROR: Invalid Header Magic" << endl;
    valid = false;
  }
  if (Version != 0x3) {
    ss << "\tERROR: Version number should be 3, is: " << Version << endl;
    valid = false;
  }
  if (IsBigEndian) {
    ss << "\tERROR: Big endian .xmf files are not supported by this importer" << endl;
    valid = false;
  }
  if (reserved0 != 0) {
    ss << "\tERROR: reserved should be 0, was " << reserved0 << endl;
    valid = false;
  }
  if (SizeOfHeader != XmfHeader::kExpectedBinarySize) {
    ss << "\tERROR: Header is not 64 bytes long!" << endl;
    valid = false;
  }
  if (DataBufferDescSize != XmfDataBufferDesc::kExpectedBinarySize) {
    ss << "\tERROR: Data Buffer Desc Size is Wrong.";
    ss << " Expected: " << std::to_string(XmfDataBufferDesc::kExpectedBinarySize);
    ss << " Actual:" << DataBufferDescSize;
    ss << endl;
    valid = false;
  }
  if (MaterialSize != XmfMaterial::kExpectedBinarySize) {
    ss << "\tERROR: Material Size is Wrong.";
    ss << " Expected: " << std::to_string(XmfMaterial::kExpectedBinarySize);
    ss << " Actual: " << MaterialSize;
    ss << endl;
    valid = false;
  }

  // TODO does this break the animations?
  if (Culling_CW != 0) {
    // Would probably still work, but I want to know about it
    ss <<
       "\tWARNING: Culling_CW != 0 (0 = CW). Is this file from a mod? if so, why are you converting it?" << endl;
  }
  if (RightHand != 0) {
    // Would probably still work, but I want to know about it
    ss <<
       "\tWARNING: RightHand != 0 (0 = D3D). Is this file from a mod? if so, why are you converting it?" << endl;
  }
  ss << "\tNumVertices: " << NumVertices << endl;
  ss << "\tNumIndices: " << NumIndices << endl;

  ss << "\tPrimitiveType: " << PrimitiveType << " (4 = TRIANGLELIST)" << endl;
  ss << "\tMeshOptimization: " << MeshOptimization << endl;
  if (MeshOptimization != 0) {
    ss << "\tWARNING: MeshOptimization is non-zero - this field is poorly understood" << endl;
  }
  ss << "\tBoundingBoxCenter: (";
  ss << BoundingBoxCenter[0] << ",";
  ss << BoundingBoxCenter[1] << ",";
  ss << BoundingBoxCenter[2] << ")" << endl;

  ss << "\tBoundingBoxSize: (";
  ss << BoundingBoxSize[0] << ",";
  ss << BoundingBoxSize[1] << ",";
  ss << BoundingBoxSize[2] << ")" << endl;

  if (PrimitiveType != D3DPT_TRIANGLELIST) {
    ss << "\tERROR: File is using a DirectX primitive type that's not supported by this importer" << endl;
    valid = false;
  }
  for (const uint8_t &b : pad) {
    if (b != 0) {
      ss << "\tERROR: Padding must be 0!" << endl;
      valid = false;
      break;
    }
  }
  std::string result = ss.str();
  if (!valid) {
    throw std::runtime_error(result);
  }
  return result;
}
}