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
        std::vector<uint8_t> compressedData;
        // TODO
//    if (reader.GetCurrentPos()- baseFileOffset != Description.DataOffset) {
//        throw std::runtime_error("Mismatching buffer data offset");
//    }
        if (reader.GetRemainingSize() < Description.GetCompressedDataSize()) {
            throw std::runtime_error(".xmf file is too small, not enough data left");
        }
        if (!Description.IsCompressed()) {
            if (Description.GetCompressedDataSize() != Description.GetUncompressedDataSize()) {
                throw std::runtime_error("Noncompressed buffer has invalid size");
            }
            uint8_t b = 0;
            for (int i = 0; i < Description.GetUncompressedDataSize(); i++) {
                reader >> b;
                compressedData.emplace_back(b);
            }
        } else {

            uint8_t b = 0;
            auto compressedDataSize = numeric_cast<unsigned long>(Description.GetCompressedDataSize());
            for (int i = 0; i < compressedDataSize; i++) {
                reader >> b;
                compressedData.emplace_back(b);
            }

            auto uncompressedSize = numeric_cast<unsigned long>(Description.GetUncompressedDataSize());
            _data.reserve(uncompressedSize);
            int status = uncompress(GetData(), &uncompressedSize, compressedData.data(), compressedDataSize);

            if (status != Z_OK) {
                throw std::runtime_error("Failed to decompress data buffer");
            }
            // Note that the second parameter of uncompress is both Input and output
            if (uncompressedSize != Description.GetUncompressedDataSize()) {
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



