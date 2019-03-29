
#include <X4ConverterTools/xmf/XmfDataBuffer.h>

using boost::numeric_cast;
namespace xmf {

    XmfDataBuffer::XmfDataBuffer() {
        Description = XmfDataBufferDesc();
    }

    XmfDataBuffer::XmfDataBuffer(Assimp::StreamReaderLE &reader) {
        Description = XmfDataBufferDesc(reader);
        std::cout << Description.validate();
        AllocData();
    }

    void XmfDataBuffer::Read(Assimp::StreamReaderLE &reader) {
        std::vector<uint8_t> compressedData;
        // TODO
//    if (reader.GetCurrentPos()- baseFileOffset != Description.DataOffset) {
//        throw std::runtime_error("Mismatching buffer data offset");
//    }
        if (reader.GetRemainingSize()
            < GetCompressedDataSize()) {
            throw std::runtime_error(".xmf file is too small, not enough data left");
        }
        if (!IsCompressed()) {
            if (GetCompressedDataSize()
                != GetUncompressedDataSize()) {
                throw std::runtime_error(
                        "Noncompressed buffer has invalid size");
            }
            uint8_t b = 0;
            for (int i = 0; i < GetUncompressedDataSize(); i++) {
                reader >> b;
                compressedData.emplace_back(b);
            }
        } else {

            uint8_t b = 0;
            unsigned long compressedDataSize = numeric_cast<unsigned long>(GetCompressedDataSize());
            for (int i = 0; i < compressedDataSize; i++) {
                reader >> b;
                compressedData.emplace_back(b);
            }

            unsigned long uncompressedSize = numeric_cast<unsigned long>(GetUncompressedDataSize());
            _data.reserve(uncompressedSize);
            int status = uncompress(GetData(), &uncompressedSize,
                                    compressedData.data(), compressedDataSize);

            if (status != Z_OK) {
                throw std::runtime_error("Failed to decompress data buffer");
            }
            // Note that the second parameter of uncompress is both Input and output
            if (uncompressedSize != GetUncompressedDataSize()) {
                throw std::runtime_error(
                        "Decompression did not return enough data");
            }
        }
    }

    void XmfDataBuffer::Write(Assimp::StreamWriterLE &writer) {
// TODO
    }

    void XmfDataBuffer::AllocData() {
        _data.resize(GetUncompressedDataSize());
    }

    bool XmfDataBuffer::IsCompressed() const {
        return Description.Compressed != 0;
    }

    bool XmfDataBuffer::IsIndexBuffer() const {
        return Description.IsIndexBuffer();
    }

    bool XmfDataBuffer::IsVertexBuffer() const {
        return Description.IsVertexBuffer();
    }

    uint64_t XmfDataBuffer::GetCompressedDataSize() const {
        return Description.CompressedDataSize;
    }

    uint64_t XmfDataBuffer::GetUncompressedDataSize() const {
        return Description.NumSections * Description.NumItemsPerSection * Description.ItemSize;
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



