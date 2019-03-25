#include <X4ConverterTools/Xmf/XmfHeader.h>

using namespace Assimp;
using namespace boost;

XmfHeader::XmfHeader(Assimp::StreamReader<> &reader) {
    for (int i = 0; i < 4; i++) {
        reader >> Magic[i];
    }
    reader >> Version >> BigEndian;
    reader >> DataBufferDescOffset;
    reader >> _pad0;
    reader >> NumDataBuffers;
    reader >> DataBufferDescSize;
    reader >> NumMaterials;
    reader >> MaterialSize;

    for (int i = 0; i < 10; i++) {
        reader >> _pad1[i];
    }
    reader >> PrimitiveType;
    validate();
}

std::string XmfHeader::validate() const {
    std::string ret;


    if (memcmp(Magic, "XUMF\x03", 5) != 0) {
        throw std::runtime_error("Invalid header magic");
    }
    if (BigEndian) {
        throw std::runtime_error(
                "Big endian .xmf files are not supported by this importer");
    }
    if (_pad0 != 0) {
        throw std::runtime_error(str(format("padding0 should be 0, was %1%") % _pad0));
    }
    if (DataBufferDescOffset != 0x40) {
        std::cout << DataBufferDescOffset << std::endl;
        throw std::runtime_error("Offset should be 0x40");
    }
    if (DataBufferDescSize > sizeof(XmfDataBufferDesc)) {
        throw std::runtime_error("Data buffer description size is too large");
    }
    if (MaterialSize != sizeof(XmfMaterial)) {
        throw std::runtime_error("Material size is invalid");
    }
    if (PrimitiveType != D3DPT_TRIANGLELIST) {
        throw std::runtime_error(
                "File is using a DirectX primitive type that's not supported by this importer");
    }
//    for (int i = 0; i < 10; ++i) {
//        if (_pad1[i] != 0){
//            throw std::runtime_error("padding should be 0!");
//        }
//    }
    return ret;
}
