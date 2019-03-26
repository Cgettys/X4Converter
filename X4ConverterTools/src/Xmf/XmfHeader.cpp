#include <X4ConverterTools/Xmf/XmfHeader.h>

using namespace Assimp;
using namespace boost;

XmfHeader::XmfHeader(Assimp::StreamReaderLE &reader) {
    for (byte &c : Magic) {
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

    for (byte &c : pad) {
        reader >> c;
    }
}

std::string XmfHeader::validate() const {
    std::string ret;


    if (memcmp(Magic, "XUMF\x03", 5) != 0) {
        throw std::runtime_error("Invalid header magic");
    }
    if (IsBigEndian) {
        throw std::runtime_error(
                "Big endian .xmf files are not supported by this importer");
    }
    if (reserved0 != 0) {
        throw std::runtime_error(str(format("padding0 should be 0, was %1%") % reserved0));
    }
    if (SizeOfHeader != XmfHeader::EXPECTED_HEADER_SIZE) {
        std::cout << SizeOfHeader << std::endl;
        throw std::runtime_error("Offset should be 0x40");
    }
    // TODO fix these
    if (DataBufferDescSize > sizeof(XmfDataBufferDesc)) {
        throw std::runtime_error("Data buffer description size is too large");
    }
    if (MaterialSize != sizeof(XmfMaterial)) {
        throw std::runtime_error("Material size is invalid");
    }

    // TODO  this and pad 2
//    for (int i = 0; i < 10; ++i) {
//        if (_pad1[i] != 0){
//            throw std::runtime_error("padding should be 0!");
//        }
//    }
    if (PrimitiveType != D3DPT_TRIANGLELIST) {
        throw std::runtime_error(
                "File is using a DirectX primitive type that's not supported by this importer");
    }
    return ret;
}

void XmfHeader::Write(Assimp::StreamWriter<> &writer) {
    for (byte &c : Magic) {
        writer << c;
    }
    writer << Version << IsBigEndian;
    writer << SizeOfHeader;
    writer << reserved0;
    writer << NumDataBuffers << DataBufferDescSize;
    writer << NumMaterials << MaterialSize;

    writer << Culling_CW << RightHand;
    writer << NumVertices << NumIndices;

    writer << PrimitiveType<<MeshOptimization ;

    for (float &f: BoundingBoxCenter) {
        writer << f;
    }

    for (float &f: BoundingBoxSize) {
        writer << f;
    }

    for (byte &c : pad) {
        writer << c;
    }
}


XmfHeader::XmfHeader(byte numDataBuffers, byte numMaterials) {
// Note that this ignores any header read into this mesh file since we can't guarantee the old header is accurate
    Magic[0] = 'X';
    Magic[1] = 'U';
    Magic[2] = 'M';
    Magic[3] = 'F';
    Version = 3;
    IsBigEndian = (byte) false;
    SizeOfHeader = 0x40;
    reserved0 = 0x00;
    NumDataBuffers = numDataBuffers;
    DataBufferDescSize = sizeof(XmfDataBufferDesc);
    NumMaterials = numMaterials;
    MaterialSize = sizeof(XmfMaterial);
    PrimitiveType = D3DPT_TRIANGLELIST;
    for (byte &c : pad) {
        c = 0x00;
    }

}
