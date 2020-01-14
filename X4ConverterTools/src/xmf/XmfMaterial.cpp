#include <X4ConverterTools/xmf/XmfMaterial.h>

using namespace boost;

namespace xmf {
XmfMaterial::XmfMaterial() {
  FirstIndex = 0;
  NumIndices = 0;
  Name.fill('\0');
}

XmfMaterial::XmfMaterial(uint32_t firstIndex, uint32_t numIndices, std::string name) {
  if (name.size() >= kMaxStrLen) {
    throw std::runtime_error(str(format("Material name %s is too long") % name));
  }
  Name.fill('\0');
  strncpy(Name.data(), name.c_str(), name.size());
  FirstIndex = firstIndex;
  NumIndices = numIndices;
}

XmfMaterial::XmfMaterial(Assimp::StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}

void XmfMaterial::ReadBinary(Assimp::StreamReaderLE &reader) {
  ReadBinaryImpl(reader);
}

void XmfMaterial::ReadBinaryImpl(Assimp::StreamReaderLE &reader) {
  reader >> FirstIndex;
  reader >> NumIndices;
  reader.CopyAndAdvance(Name.data(), kMaxStrLen);
}

void XmfMaterial::WriteBinary(Assimp::StreamWriterLE &writer) const {
  writer << FirstIndex;
  writer << NumIndices;
  for (auto &c : Name) {
    writer << c;
  }
}
}
