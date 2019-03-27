#include <X4ConverterTools/Xmf/XmfHeader.h>

using namespace Assimp;
using namespace boost;

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
    // TODO

    Culling_CW;
    RightHand;
    NumVertices;
    NumIndices;


    PrimitiveType = D3DPT_TRIANGLELIST;
    for (byte &c : pad) {
        c = 0x00;
    }
    // TODO verify this
    MeshOptimization = 0;

}

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


void XmfHeader::Write(Assimp::StreamWriterLE &writer) {
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

    writer << PrimitiveType << MeshOptimization;

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


std::string XmfHeader::validate() const {
    std::string ret;
    bool valid = true;

    // TODO prettyprinter

    if (memcmp(Magic, "XUMF\x03", 5) != 0) {
        ret.append("\tERROR: Invalid Header Magic or Version\n");
        valid = false;
    }
    if (IsBigEndian) {
        ret.append("\tERROR: Big endian .xmf files are not supported by this importer\n");
        valid = false;
    }
    if (reserved0 != 0) {
        ret.append(str(format("\tERROR: reserved should be 0, was %1%\n") % reserved0));
        valid = false;
    }
    if (SizeOfHeader != XmfHeader::EXPECTED_HEADER_SIZE) {
        ret.append("\tERROR: Header is not 64 bytes long!\n");
        valid = false;
    }
    // TODO fix these
//    if (DataBufferDescSize > sizeof(XmfDataBufferDesc)) {
//        throw std::runtime_error("Data buffer description size is too large");
//    }
//    if (MaterialSize != sizeof(XmfMaterial)) {
//        throw std::runtime_error("Material size is invalid");
//    }

    if (Culling_CW != 0) {
        ret.append("\tWARNING: Culling_CW should be 0, was not 0\n");
    }
    if (RightHand != 0) {
        ret.append("\tWARNING: RightHand != 0 (0 = D3D)\n");
    }
    ret.append(str(format("\tNumVertices: %1%\n") % NumVertices));
    ret.append(str(format("\tNumIndices: %1%\n") % NumIndices));

    ret.append(str(format("\tPrimitiveType: %1% (4 = TRIANGLELIST)\n")%PrimitiveType));
    ret.append(str(format("\tMeshOptimization: %1%\n")%MeshOptimization));
    if (MeshOptimization != 0){
        ret.append("\tWARNING: Field poorly understood");
    }
    ret.append(str(format("\tBoundingBoxCenter: (%1%,%2%,%3%)\n") % BoundingBoxCenter[0] % BoundingBoxCenter[1] %
                   BoundingBoxCenter[2]));

    ret.append(str(format("\tBoundingBoxSize: (%1%,%2%,%3%)\n") % BoundingBoxSize[0] % BoundingBoxSize[1] %
                   BoundingBoxSize[2]));


    if (PrimitiveType != D3DPT_TRIANGLELIST) {
        ret.append("\tERROR: File is using a DirectX primitive type that's not supported by this importer\n");
        valid = false;
    }
    for (const byte &b : pad) {
        if (b != 0) {
            ret.append("\tERROR: Padding must be 0!\n");
            valid = false;
            break;
        }
    }
    if (!valid) {
        throw std::runtime_error(ret);
    }
    return ret;
}