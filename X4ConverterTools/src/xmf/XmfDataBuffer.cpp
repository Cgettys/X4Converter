#include <X4ConverterTools/xmf/XmfDataBuffer.h>
#include <boost/numeric/conversion/cast.hpp>
#include "zlib.h"

using boost::numeric_cast;
namespace xmf {

XmfDataBuffer::XmfDataBuffer() {
  Description = XmfDataBufferDesc();
}

XmfDataBuffer::XmfDataBuffer(Assimp::StreamReaderLE &reader) {
  Description = XmfDataBufferDesc(reader);
  Description.validate();
//        std::cout <<
  AllocData();
}

void XmfDataBuffer::ReadBinary(Assimp::StreamReaderLE &reader) {
  // TODO
//    if (reader.GetCurrentPos()- baseFileOffset != Description.DataOffset) {
//        throw std::runtime_error("Mismatching buffer data offset");
//    }
  auto compressed_size = Description.GetCompressedDataSize();
  auto uncompressed_size = Description.GetUncompressedDataSize();
  if (reader.GetRemainingSize() < compressed_size) {
    throw std::runtime_error(".xmf file is too small, not enough data left");
  }
  if (!Description.IsCompressed()) {
    if (compressed_size != uncompressed_size) {
      throw std::runtime_error("Uncompressed buffer has invalid size");
    }
    reader.CopyAndAdvance(GetData(), uncompressed_size);
  } else {
    std::vector<uint8_t> compressed_data(compressed_size);
    reader.CopyAndAdvance(compressed_data.data(), compressed_size);

    auto compressed_size_tmp = numeric_cast<unsigned long>(compressed_size);
    auto uncompressed_size_tmp = numeric_cast<unsigned long>(uncompressed_size);
    int status = uncompress(GetData(), &uncompressed_size_tmp, compressed_data.data(), compressed_size_tmp);

    if (status != Z_OK) {
      throw std::runtime_error("Failed to decompress data buffer");
    }
// Note that the second parameter of uncompress is both Input and output
    if (uncompressed_size_tmp != uncompressed_size) {
      throw std::runtime_error("Decompression did not return enough data");
    }
  }
}

void XmfDataBuffer::WriteBinary(Assimp::StreamWriterLE &writer) const {
  for (auto b : _data) {
    writer << b;
  }
// TODO
}

void XmfDataBuffer::AllocData() {
  _data.resize(Description.GetUncompressedDataSize());
}

bool XmfDataBuffer::IsIndexBuffer() const {
  return Description.IsIndexBuffer();
}

bool XmfDataBuffer::IsVertexBuffer() const {
  return Description.IsVertexBuffer();
}

uint8_t *XmfDataBuffer::GetData() {
//    auto copiedData = new byte[_data.size()];
//    std::copy(_data.begin(), _data.end(), copiedData);
//    return copiedData;
  return _data.data();
}

D3DFORMAT XmfDataBuffer::GetIndexFormat() {
  return Description.GetIndexFormat();
}
}



