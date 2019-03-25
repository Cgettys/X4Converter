
#include <X4ConverterTools/Xmf/XmfDataBuffer.h>


XmfDataBuffer::XmfDataBuffer() {
}

XmfDataBuffer::XmfDataBuffer(Assimp::StreamReader<> &reader) :
        Description(reader) {

    AllocData();
}
void XmfDataBuffer::Read(Assimp::StreamReader<> & reader){
    std::vector<byte> compressedData;
    // TODO fixme
//    if (reader.GetCurrentPos()- baseFileOffset != Description.DataOffset) {
//        throw std::runtime_error("Mismatching buffer data offset");
//    }
    // TODO fixme
//        if (pStreamReader. - pStreamReader.GetCurrentPos()
//            < GetCompressedDataSize()) {
//            throw std::runtime_error(".xmf file is too small");
//        }

    if (!IsCompressed()) {
        if (GetCompressedDataSize()
            != GetUncompressedDataSize()) {
            throw std::runtime_error(
                    "Noncompressed buffer has invalid size");
        }
        reader.CopyAndAdvance(GetData(), GetUncompressedDataSize());
    } else {
        compressedData.reserve(GetCompressedDataSize());
        reader.CopyAndAdvance(GetData(), GetCompressedDataSize());

        unsigned long uncompressedSize = GetUncompressedDataSize();
        int status = uncompress(GetData(), &uncompressedSize,
                                compressedData.data(), GetCompressedDataSize());
        if (status != Z_OK) {
            throw std::runtime_error("Failed to decompress data buffer");
        }
        if (uncompressedSize != GetUncompressedDataSize()) {
            throw std::runtime_error(
                    "Decompression did not return enough data");
        }
    }
}
void XmfDataBuffer::Write(Assimp::StreamWriter<> &writer) {
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

int XmfDataBuffer::GetCompressedDataSize() const {
    return Description.CompressedDataSize;
}

int XmfDataBuffer::GetUncompressedDataSize() const {
    return Description.NumSections * Description.NumItemsPerSection * Description.ItemSize;
}

byte *XmfDataBuffer::GetData() {
//    auto copiedData = new byte[_data.size()];
//    std::copy(_data.begin(), _data.end(), copiedData);
//    return copiedData;
    return _data.data();
}

D3DFORMAT XmfDataBuffer::GetIndexFormat() {
    return Description.GetIndexFormat();
}



